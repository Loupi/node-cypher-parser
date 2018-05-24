#include <nan.h>
//#include "parser.hpp"

using namespace Nan;
using namespace std;
using namespace v8;

class ParserWorker : public AsyncWorker {
public:
  ParserWorker(string query, Callback *callback)
  : AsyncWorker(callback), query(query) {}

  ~ParserWorker() {}

  void Execute () {
    json = "{\"a\": 1}";
    // succeeded = NodeBin::Parse(json, query);
  }

  // We have the results, and we're back in the event loop.
  void HandleOKCallback () {
    Nan::HandleScope scope;
    Nan::JSON NanJSON;
    auto inp = New(json.c_str());
    if (!inp.IsEmpty()) {
      Nan::MaybeLocal<v8::Value> result = NanJSON.Parse(inp.ToLocalChecked());
      Local<Value> argv[] = {
        Null(),
        result.ToLocalChecked()
      };
      AsyncResource resource("cypher-parser-callback");
      resource.runInAsyncScope(GetCurrentContext()->Global(), **callback, 2, argv);
    }
  }
  
private:
  string json;
  string query;
  bool succeeded;
};

NAN_METHOD(Parse) {
  Utf8String query(info[0]->ToString());
  Callback *callback = new Callback(info[1].As<Function>());

  AsyncQueueWorker(new ParserWorker(*query, callback));
}

NAN_MODULE_INIT(InitAll) {
  Export(target, "parse", Parse);
}

NODE_MODULE(Cypher, InitAll)