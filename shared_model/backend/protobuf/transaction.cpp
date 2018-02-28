/**
 * Copyright Soramitsu Co., Ltd. 2018 All Rights Reserved.
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

#include "backend/protobuf/transaction.hpp"

#include "block.pb.h"

namespace shared_model {
  namespace proto {

    interface::types::TimestampType proto::Transaction::createdTime() const {
      return payload_.created_time();
    }

    bool proto::Transaction::addSignature(
        const interface::types::SignatureType &signature) {
      if (signatures_->count(signature) > 0) {
        return false;
      }
      auto sig = proto_->add_signature();
      sig->set_pubkey(crypto::toBinaryString(signature->publicKey()));
      sig->set_signature(crypto::toBinaryString(signature->signedData()));
      signatures_.invalidate();
      return true;
    }

    const interface::SignatureSetType &Transaction::signatures() const {
      return *signatures_;
    }

    const Transaction::HashType &Transaction::hash() const {
      return *txhash_;
    }

    const Transaction::BlobType &Transaction::payload() const {
      return *blobTypePayload_;
    }

    const Transaction::BlobType &Transaction::blob() const {
      return *blob_;
    }

    const Transaction::CommandsType &Transaction::commands() const {
      return *commands_;
    }

    interface::types::CounterType Transaction::transactionCounter() const {
      return payload_.tx_counter();
    }

    const interface::types::AccountIdType &Transaction::creatorAccountId()
        const {
      return payload_.creator_account_id();
    }

    Transaction::Transaction(Transaction &&o) noexcept
        : Transaction(std::move(o.proto_)) {}

    Transaction::Transaction(const Transaction &o) : Transaction(o.proto_) {}

    //< explicit specialization
    template <>
    Transaction::Transaction(const iroha::protocol::Transaction &tx);
    template <>
    Transaction::Transaction(iroha::protocol::Transaction &&tx);
  }
}
