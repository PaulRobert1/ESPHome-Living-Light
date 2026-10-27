#pragma once
#include <cstdint>
class WallSwitchLogic {
 public:
  enum class Action { NONE, BULB, RELAY_ON, RELAY_OFF };
  Action update(uint32_t now, bool on_position) {
    if (!initialized_) {
      initialized_ = true;
      baseline_ = previous_ = on_position;
      return Action::NONE;
    }
    Action result = Action::NONE;
    if (pending_ && uint32_t(now - started_) > 2000) {
      baseline_ = previous_;
      pending_ = false;
      result = baseline_ ? Action::RELAY_ON : Action::RELAY_OFF;
      acted_ = true;
      last_action_ = now;
    }
    if (on_position != previous_) {
      previous_ = on_position;
      if (on_position != baseline_) {
        pending_ = true;
        started_ = now;
        suppressed_ = acted_ && uint32_t(now - last_action_) < 500;
      } else if (pending_) {
        pending_ = false;
        if (!suppressed_ && (!acted_ || uint32_t(now - last_action_) >= 500)) {
          result = Action::BULB;
          acted_ = true;
          last_action_ = now;
        }
      }
    }
    return result;
  }
 private:
  bool initialized_{false}, baseline_{false}, previous_{false};
  bool pending_{false}, suppressed_{false}, acted_{false};
  uint32_t started_{0}, last_action_{0};
};
