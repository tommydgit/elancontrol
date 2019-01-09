#include <node_api.h>

napi_value MyFunction(napi_env env, napi_callback_info info) {
  napi_status status;
  size_t argc = 2;
  int channel = 0;
  int command = 0;
  napi_value argv[1];
  status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Failed to parse arguments");
  }
  status = napi_get_value_int32(env, argv[0], &channel);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid channel was passed as argument");
  }
  status = napi_get_value_int32(env, argv[1], &command);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Invalid command was passed as argument");
  }  
  napi_value return_code;
  //Code Here
  return_code = channel + command; //Test shell for API
  status = napi_create_int32(env, number, &return_code);

  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to create return value");
  }

  return return_code;
}

napi_value Init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value fn;

  status = napi_create_function(env, NULL, 0, MyFunction, NULL, &fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to wrap native function");
  }

  status = napi_set_named_property(env, exports, "my_function", fn);
  if (status != napi_ok) {
    napi_throw_error(env, NULL, "Unable to populate exports");
  }

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)