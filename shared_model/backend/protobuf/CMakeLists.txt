add_library(sm_proto_transaction OBJECT
  transaction.cpp
  )


add_library(shared_model_proto_backend
  impl.cpp

  $<TARGET_OBJECTS:sm_proto_transaction>
  )

target_link_libraries(shared_model_proto_backend
  schema
  model_interfaces
  )
