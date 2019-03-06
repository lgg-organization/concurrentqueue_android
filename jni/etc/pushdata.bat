
adb push concurrentqueue_test /system/bin/.
adb shell chmod 777 /system/bin/concurrentqueue_test
adb shell ls -l /system/bin/concurrentqueue_test

adb shell "/system/bin/concurrentqueue_test" && adb logcat -s ANDROID_TEST