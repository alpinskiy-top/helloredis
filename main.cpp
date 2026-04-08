#include <cstdlib>
#include <iostream>
#include <string>

#include <sw/redis++/redis++.h>

namespace {

std::string default_redis_uri() {
  const char *env = std::getenv("REDIS_URI");
  if (env && env[0] != '\0') {
    return env;
  }
  return "tcp://127.0.0.1:6379";
}

void print_usage(const char *prog) {
  std::cerr << "Usage: " << prog << " [--uri URI] [QUEUE_KEY]\n"
            << "\n"
            << "Consumes messages from a Redis LIST using BLPOP and prints "
               "each payload\n"
            << "to stdout (one line per message).\n"
            << "\n"
            << "  --uri URI      Redis connection URI (overrides REDIS_URI)\n"
            << "  QUEUE_KEY      List key to blocking-pop (default: queue)\n"
            << "\n"
            << "Environment:\n"
            << "  REDIS_URI      Default tcp://127.0.0.1:6379\n";
}

bool parse_args(int argc, char *argv[], std::string &uri,
                std::string &queue_key, bool &want_help) {
  uri = default_redis_uri();
  queue_key = "queue";
  want_help = false;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "-h" || arg == "--help") {
      want_help = true;
      return false;
    }
    if (arg.rfind("--uri=", 0) == 0) {
      uri = arg.substr(6);
      continue;
    }
    if (arg == "--uri") {
      if (i + 1 >= argc) {
        std::cerr << "error: --uri requires a value\n";
        return false;
      }
      uri = argv[++i];
      continue;
    }
    if (arg[0] == '-') {
      std::cerr << "error: unknown option " << arg << "\n";
      return false;
    }
    queue_key = arg;
  }
  return true;
}

} // namespace

int main(int argc, char *argv[]) {
  std::string uri;
  std::string queue_key;
  bool want_help = false;
  if (!parse_args(argc, argv, uri, queue_key, want_help)) {
    print_usage(argv[0]);
    return want_help ? 0 : 1;
  }
  try {
    auto connection_options = sw::redis::Uri(uri).connection_options();
    std::cout << "Connect..." << std::endl;
    auto redis = sw::redis::Redis(connection_options);
    std::cout << "Subscribe..." << std::endl;
    auto subscriber = redis.subscriber();
    subscriber.on_message(
        [](const std::string &channel, const std::string &value) {
          std::cout << value << std::endl;
        });
    subscriber.subscribe(queue_key);
    std::cout << "Ready" << std::endl;
    while (true) {
      subscriber.consume();
    }
  } catch (const sw::redis::Error &e) {
    std::cerr << "redis error: " << e.what() << '\n';
    return 1;
  }
}
