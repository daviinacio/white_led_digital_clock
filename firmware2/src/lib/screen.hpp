#ifndef WLDC_SCREEN_CLASS_H
#define WLDC_SCREEN_CLASS_H

template <typename T>
class Screen
{
public:
  int refreshInterval = 500;

  virtual ~Screen() = default;
  virtual void onCreate() {};
  virtual void onRender() {};
  virtual void onDestroy() {};
};

#endif
