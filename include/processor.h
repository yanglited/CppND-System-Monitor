#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();
 private:
  float prevIdle {0.0f};
  float prevTotal {0.0f};
};

#endif