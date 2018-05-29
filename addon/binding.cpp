#include <nan.h>
#include "parser.hpp"

using namespace Nan;
using namespace std;
using namespace v8;

class CypherParserWorker : public AsyncWorker {
public:
  CypherParserWorker(const string& query, unsigned int width, bool dumpAst, bool colorize, Callback *callback)
  : AsyncWorker(callback), query(query), width(width), dumpAst(dumpAst), colorize(colorize) {}

  ~CypherParserWorker() {}

  void Execute () {
    succeeded = NodeBin::Parse(json, query, width, dumpAst, colorize);
  }
  
  void HandleOKCallback () {
    Nan::HandleScope scope;
    Nan::JSON NanJSON;
    auto json = New(this->json.c_str());
    if (!json.IsEmpty()) {
      Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(json.ToLocalChecked());
      Nan::MaybeLocal<v8::Value> succeeded = Nan::New(this->succeeded);

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
  bool colorize;
  string json;
  bool succeeded;
};

NAN_METHOD(Parse) {
  Nan::HandleScope scope; 
  Local<Value> query;
  unsigned int width = 0;
  bool dumpAst = false;
  bool colorize = false;

  if (info.Length() < 2) {
    ThrowError("Missing parameters.");
    return;
  }

  if (!info[1]->IsFunction()) {
    ThrowError("Parameter callback must be a function.");
    return;
  }

  if (info[0]->IsString()) {
    query = info[0];
  }
  else if (info[0]->IsObject()) {
    auto object = info[0]->ToObject();

    auto key = Nan::New("query").ToLocalChecked();
    if (object->Has(key)) {
      auto val = object->Get(key);
      if (!val->IsString()) {
        ThrowError("Property query must be a string.");
        return;
      }
      query = val;
    }

    key = Nan::New("width").ToLocalChecked();
    if (object->Has(key)) {
      auto val = object->Get(key);
      if (!val->IsNumber()) {
        ThrowError("Property width must be a number.");
        return;
      }
      width = val->Uint32Value();
    }

    key = Nan::New("dumpAst").ToLocalChecked();
    if (object->Has(key)) {
      auto val = object->Get(key);
      if (!val->IsBoolean()) {
        ThrowError("Property dumpAst must be a boolean.");
        return;
      }
      dumpAst = val->BooleanValue();
    }

    key = Nan::New("colorize").ToLocalChecked();
    if (object->Has(key)) {
      auto val = object->Get(key);
      if (!val->IsBoolean()) {
        ThrowError("Property colorize must be a boolean.");
        return;
      }
      colorize = val->BooleanValue();
    }
  }
  else {
    ThrowError("Parameter query must be an object or a string.");
    return;
  }

  Utf8String uftStr(query->ToString());
  Callback *callback = new Callback(info[1].As<Function>());
  AsyncQueueWorker(new CypherParserWorker(*uftStr, width, dumpAst, colorize, callback));
}

NAN_MODULE_INIT(InitAll) {
  Export(target, "parse", Parse);
}

NODE_MODULE(Cypher, InitAll)