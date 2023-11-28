#ifndef SESSION_H
#define SESSION_H

#include <cstddef>
#include <cstdint>

class Session {
 protected:
  uint32_t numErrors = 0;
  uint32_t currentTrial = 1;
  bool onGoing = true;
  uint32_t userId;
  uint32_t wordLen;
  uint32_t maxErrors;

 public:
  uint32_t getuserId();
  uint32_t getWordLen();
  uint32_t getMaxErrors();
  uint32_t getCurrentTrial();
  uint32_t getGoodTrials();
  uint32_t getNumErrors();
  bool isOnGoing();
  void finishAuction();  // setOnGoing(false)
};

#endif