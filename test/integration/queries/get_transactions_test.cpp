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

#include <gtest/gtest.h>
#include "backend/protobuf/transaction.hpp"
#include "builders/protobuf/queries.hpp"
#include "cryptography/crypto_provider/crypto_defaults.hpp"
#include "datetime/time.hpp"
#include "framework/base_tx.hpp"
#include "framework/integration_framework/integration_test_framework.hpp"
#include "interfaces/utils/query_error_response_visitor.hpp"
#include "model/permissions.hpp"

using namespace std::string_literals;
using namespace integration_framework;
using namespace shared_model;

class GetTransaction : public ::testing::Test {
 public:
  const std::string kUser = "user"s;
  const std::string kNewRole = "rl"s;
  const std::string kUserId = kUser + "@test";
  const crypto::Keypair kAdminKeypair =
      crypto::DefaultCryptoAlgorithmType::generateKeypair();
  const crypto::Keypair kUserKeypair =
      crypto::DefaultCryptoAlgorithmType::generateKeypair();
};

TEST_F(GetTransaction, HaveNoGetPerms) {
  auto unsigned_tx =
      framework::createUserWithPerms(kUser,
                                     kUserId,
                                     kUserKeypair.publicKey(),
                                     kNewRole,
                                     {iroha::model::can_read_assets})
          .build();
  auto tx = unsigned_tx.signAndAddSignature(kAdminKeypair);
  auto qry = proto::QueryBuilder()
                 .createdTime(iroha::time::now())
                 .creatorAccountId(kUserId)
                 .queryCounter(1)
                 .getTransactions(std::vector<crypto::Hash>{unsigned_tx.hash()})
                 .build()
                 .signAndAddSignature(kUserKeypair);

  auto check = [](auto &status) {
    ASSERT_TRUE(
        boost::apply_visitor(interface::QueryErrorResponseChecker<
                                 interface::StatefulFailedErrorResponse>(),
                             status.get()));
  };

  IntegrationTestFramework()
      .setInitialState(kAdminKeypair)
      .sendTx(tx)
      .checkProposal(
          [](auto &block) { ASSERT_EQ(block->transactions.size(), 1); })
      .sendQuery(qry, check)
      .done();
}
