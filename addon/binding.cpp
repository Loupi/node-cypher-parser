#include <nan.h>
#include "parser.hpp"

using namespace Nan;
using namespace std;
using namespace v8;

class CypherParserWorker : public AsyncWorker {
public:
  CypherParserWorker(const string& query, unsigned int width, bool dumpAst, bool rawJson, bool colorize, Callback *callback)
  : AsyncWorker(callback), query(query), width(width), dumpAst(dumpAst), rawJson(rawJson), colorize(colorize) {}

  ~CypherParserWorker() {}

  void Execute () {
    succeeded = NodeBin::Parse(json, query, width, dumpAst, colorize);
  }
  
  void HandleOKCallback () {
    Nan::HandleScope scope;
    Nan::JSON NanJSON;
    auto json = New(this->json.c_str());
    if (!json.IsEmpty()) {
      Nan::MaybeLocal<v8::Value> succeeded = Nan::New(this->succeeded);
      Nan::MaybeLocal<v8::Value> result;

      if (rawJson)
        result = json.ToLocalChecked();
      else
        result = NanJSON.Parse(json.ToLocalChecked());

      Local<Value> argv[] = {
        succeeded.ToLocalChecked(),
        result.ToLocalChecked()
      };
      AsyncResource resource("cypher-parser-callback");
      resource.runInAsyncScope(GetCurrentContext()->Global(), **callback, 2, argv);
    }
  }
  
private:
  string query;
  unsigned int width;
  bool dumpAst;
  bool rawJson;
  bool colorize;
  string json;
  bool succeeded;
};

Local<Value> GetOptionalStringParam(const char* name, Local<Object>& object, Local<Value>& defaultValue) {
  auto key = Nan::New(name).ToLocalChecked();
  if (object->Has(Nan::GetCurrentContext(), key).FromJust()) {
    auto val = object->Get(Nan::GetCurrentContext(), key).ToLocalChecked();
    if (!val->IsString()) {
      std::string msg = "Property ";
      msg += name;
      msg += " must be a string.";
      ThrowError(msg.c_str());
      return defaultValue;
    }
    return val;
  }
  return defaultValue;
}

unsigned int GetOptionalUIntParam(const char* name, Local<Object>& object, unsigned int defaultValue) {
  auto key = Nan::New(name).ToLocalChecked();
  if (object->Has(Nan::GetCurrentContext(), key).FromJust()) {
    auto val = object->Get(Nan::GetCurrentContext(), key).ToLocalChecked();
    if (!val->IsNumber()) {
      std::string msg = "Property ";
      msg += name;
      msg += " must be a number.";
      ThrowError(msg.c_str());
      return defaultValue;
    }
    return val->Uint32Value(Nan::GetCurrentContext()).FromJust();
  }
  return defaultValue;
}

bool GetOptionalBoolParam(const char* name, Local<Object>& object, bool defaultValue) {
  auto key = Nan::New(name).ToLocalChecked();
  if (object->Has(Nan::GetCurrentContext(), key).FromJust()) {
    auto val = object->Get(Nan::GetCurrentContext(), key).ToLocalChecked();
    if (!val->IsBoolean()) {
      std::string msg = "Property ";
      msg += name;
      msg += " must be a boolean.";
      ThrowError(msg.c_str());
      return defaultValue;
    }
    return *val;
  }
  return defaultValue;
}


NAN_METHOD(Parse) {
  Nan::HandleScope scope; 
  Local<Value> query;
  unsigned int width = 0;
  bool dumpAst = false;
  bool rawJson = false;
  bool colorize = false;

  if (info.Length() < 2) {
    ThrowError("Missing parameters.");
    return;
  }

  if (!info[0]->IsFunction()) {
    ThrowError("Parameter callback must be a function.");
    return;
  }

  if (info[1]->IsString()) {
    query = info[1];
  }
  else if (info[0]->IsObject()) {
    auto object = info[1]->ToObject(Nan::GetCurrentContext()).ToLocalChecked();
    query = GetOptionalStringParam("query", object, query);
    width = GetOptionalUIntParam("width", object, width);
    dumpAst = GetOptionalBoolParam("dumpAst", object, dumpAst);
    rawJson = GetOptionalBoolParam("rawJson", object, rawJson);
    colorize = GetOptionalBoolParam("colorize", object, colorize);
  }
  else {
    ThrowError("Parameter query must be an object or a string.");
    return;
  }

  Utf8String uftStr(query->ToString(Nan::GetCurrentContext()).ToLocalChecked());
  Callback *callback = new Callback(info[0].As<Function>());
  AsyncQueueWorker(new CypherParserWorker(*uftStr, width, dumpAst, rawJson, colorize, callback));
}

NAN_MODULE_INIT(InitAll) {
  Export(target, "parse", Parse);
}

NODE_MODULE(Cypher, InitAll)