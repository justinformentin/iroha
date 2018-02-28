/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP
#define IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP

#include "interfaces/transaction.hpp"

#include <boost/range/numeric.hpp>

#include "backend/protobuf/commands/proto_command.hpp"
#include "backend/protobuf/common_objects/signature.hpp"
#include "utils/lazy_initializer.hpp"

namespace iroha {
  namespace protocol {
    class Transaction;
  }
}

namespace shared_model {
  namespace proto {

    class Transaction FINAL : public CopyableProto<interface::Transaction,
                                                   iroha::protocol::Transaction,
                                                   Transaction> {
     private:
      /** workaround to make {l,r}value constructors without code duplication
       * **/
      template <typename TransactionType>
      Transaction(TransactionType &&transaction)
          : CopyableProto(std::forward<TransactionType>(transaction)),
            payload_(proto_->payload()),
            commands_([this] {
              return boost::accumulate(
                  payload_.commands(),
                  CommandsType{},
                  [](auto &&acc, const auto &cmd) {
                    acc.emplace_back(new Command(cmd));
                    return std::forward<decltype(acc)>(acc);
                  });
            }),
            blob_([this] { return makeBlob(*proto_); }),
            blobTypePayload_([this] { return makeBlob(payload_); }),
            signatures_([this] {
              return boost::accumulate(
                  proto_->signature(),
                  interface::SignatureSetType{},
                  [](auto &&acc, const auto &sig) {
                    acc.emplace(new Signature(sig));
                    return std::forward<decltype(acc)>(acc);
                  });
            }),
            txhash_([this] { return HashProviderType::makeHash(payload()); }) {}

     public:
      //< initializing constructor, explicitly specified in cpp
      Transaction(const iroha::protocol::Transaction &tx);

      //< initializing constructor, explicitly specified in cpp
      Transaction(iroha::protocol::Transaction &&tx);

      //< copy constructor
      Transaction(const Transaction &o);

      //< move constructor
      Transaction(Transaction &&o) noexcept;

      const interface::types::AccountIdType &creatorAccountId() const override;

      interface::types::CounterType transactionCounter() const override;

      const Transaction::CommandsType &commands() const override;

      const Transaction::BlobType &blob() const override;

      const Transaction::BlobType &payload() const override;

      const Transaction::HashType &hash() const override;

      const interface::SignatureSetType &signatures() const override;

      bool addSignature(
          const interface::types::SignatureType &signature) override;

      interface::types::TimestampType createdTime() const override;

     private:
      // lazy
      template <typename T>
      using Lazy = detail::LazyInitializer<T>;

      const iroha::protocol::Transaction::Payload &payload_;

      const Lazy<CommandsType> commands_;

      const Lazy<BlobType> blob_;

      const Lazy<BlobType> blobTypePayload_;

      const Lazy<interface::SignatureSetType> signatures_;

      const Lazy<HashType> txhash_;
    };
  }  // namespace proto
}  // namespace shared_model

#endif  // IROHA_SHARED_MODEL_PROTO_TRANSACTION_HPP
