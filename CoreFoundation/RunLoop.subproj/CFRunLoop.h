/*	CFRunLoop.h
	Copyright (c) 1998-2018, Apple Inc. and the Swift project authors
 
	Portions Copyright (c) 2014-2018, Apple Inc. and the Swift project authors
	Licensed under Apache License v2.0 with Runtime Library Exception
	See http://swift.org/LICENSE.txt for license information
	See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
*/

#if !defined(__COREFOUNDATION_CFRUNLOOP__)
#define __COREFOUNDATION_CFRUNLOOP__ 1

#include <CoreFoundation/CFBase.h>
#include <CoreFoundation/CFArray.h>
#include <CoreFoundation/CFDate.h>
#include <CoreFoundation/CFString.h>
#if (TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE)) || (TARGET_OS_EMBEDDED || TARGET_OS_IPHONE)
#include <mach/port.h>
#endif

CF_IMPLICIT_BRIDGING_ENABLED
CF_EXTERN_C_BEGIN

// 定义 各种类型
typedef CFStringRef CFRunLoopMode CF_EXTENSIBLE_STRING_ENUM;

typedef struct CF_BRIDGED_MUTABLE_TYPE(id) __CFRunLoop * CFRunLoopRef;

typedef struct CF_BRIDGED_MUTABLE_TYPE(id) __CFRunLoopSource * CFRunLoopSourceRef;

typedef struct CF_BRIDGED_MUTABLE_TYPE(id) __CFRunLoopObserver * CFRunLoopObserverRef;

typedef struct CF_BRIDGED_MUTABLE_TYPE(NSTimer) __CFRunLoopTimer * CFRunLoopTimerRef;

/* Reasons for CFRunLoopRunInMode() to Return */

// CFRunLoopInMode() 返回的原因
typedef CF_ENUM(SInt32, CFRunLoopRunResult) {
    kCFRunLoopRunFinished = 1,              //runloop中已经没有sources和timers
    kCFRunLoopRunStopped = 2,           //runloop通过 CFRunLoopStop(_:)方法停止
    kCFRunLoopRunTimedOut = 3,              //runloop设置的时间已到
    kCFRunLoopRunHandledSource = 4      // 当returnAfterSourceHandled值为ture时，一个source被执行完
};

/* Run Loop Observer Activities */
typedef CF_OPTIONS(CFOptionFlags, CFRunLoopActivity) {
    kCFRunLoopEntry = (1UL << 0),           // 即将进入loop
    kCFRunLoopBeforeTimers = (1UL << 1),    // 即将处理Timer
    kCFRunLoopBeforeSources = (1UL << 2),   // 即将处理source
    kCFRunLoopBeforeWaiting = (1UL << 5),   // 即将进入休眠
    kCFRunLoopAfterWaiting = (1UL << 6),    // 从休眠中唤醒
    kCFRunLoopExit = (1UL << 7),            // 即将推出rloop
    kCFRunLoopAllActivities = 0x0FFFFFFFU
};

# pragma mark - runloop的mode
// extern
CF_EXPORT const CFRunLoopMode kCFRunLoopDefaultMode;                // C 语言此处只给出了两种模式   
CF_EXPORT const CFRunLoopMode kCFRunLoopCommonModes; //虚拟的   包括：kCFRunLoopDefaultMode + UITrackingRunLoopMode
// 获取runloop的唯一ID(此处ID 未公开，但是可以拿到)
CF_EXPORT CFTypeID CFRunLoopGetTypeID(void);


// 获取runloop
CF_EXPORT CFRunLoopRef CFRunLoopGetCurrent(void);
CF_EXPORT CFRunLoopRef CFRunLoopGetMain(void);

// 当前正在执行的 mode
CF_EXPORT CFRunLoopMode CFRunLoopCopyCurrentMode(CFRunLoopRef rl);

// 获取runloop支持的所有mode
CF_EXPORT CFArrayRef CFRunLoopCopyAllModes(CFRunLoopRef rl);

// 添加 Common 模式, 此处操作挺多的，详细看源码
CF_EXPORT void CFRunLoopAddCommonMode(CFRunLoopRef rl, CFRunLoopMode mode);

CF_EXPORT CFAbsoluteTime CFRunLoopGetNextTimerFireDate(CFRunLoopRef rl, CFRunLoopMode mode);



// Runloop的管理
CF_EXPORT void CFRunLoopRun(void);
CF_EXPORT CFRunLoopRunResult CFRunLoopRunInMode(CFRunLoopMode mode, CFTimeInterval seconds, Boolean returnAfterSourceHandled);
//
CF_EXPORT Boolean CFRunLoopIsWaiting(CFRunLoopRef rl);
CF_EXPORT void CFRunLoopWakeUp(CFRunLoopRef rl);
// stop 终止： 这里有点意思， 它只是终止了 currentMode， 并不是停止整个runloop， 在currentMode 终止成功后， 会再次尝试唤醒RunLoop（调用CFRunLoopWakeUp）
CF_EXPORT void CFRunLoopStop(CFRunLoopRef rl);

//                                  RunLoop 方法      ·


#if __BLOCKS__
// 向特定的模式中添加 block
// 该方式不会直接唤醒 runloop。在下次被唤醒时执行 
CF_EXPORT void CFRunLoopPerformBlock(CFRunLoopRef rl, CFTypeRef mode, void (^block)(void)) API_AVAILABLE(macos(10.6), ios(4.0), watchos(2.0), tvos(9.0)); 
#endif

CF_EXPORT Boolean CFRunLoopContainsSource(CFRunLoopRef rl, CFRunLoopSourceRef source, CFRunLoopMode mode);
CF_EXPORT void CFRunLoopAddSource(CFRunLoopRef rl, CFRunLoopSourceRef source, CFRunLoopMode mode);
CF_EXPORT void CFRunLoopRemoveSource(CFRunLoopRef rl, CFRunLoopSourceRef source, CFRunLoopMode mode);

CF_EXPORT Boolean CFRunLoopContainsObserver(CFRunLoopRef rl, CFRunLoopObserverRef observer, CFRunLoopMode mode);
CF_EXPORT void CFRunLoopAddObserver(CFRunLoopRef rl, CFRunLoopObserverRef observer, CFRunLoopMode mode);
CF_EXPORT void CFRunLoopRemoveObserver(CFRunLoopRef rl, CFRunLoopObserverRef observer, CFRunLoopMode mode);

CF_EXPORT Boolean CFRunLoopContainsTimer(CFRunLoopRef rl, CFRunLoopTimerRef timer, CFRunLoopMode mode);
CF_EXPORT void CFRunLoopAddTimer(CFRunLoopRef rl, CFRunLoopTimerRef timer, CFRunLoopMode mode);
CF_EXPORT void CFRunLoopRemoveTimer(CFRunLoopRef rl, CFRunLoopTimerRef timer, CFRunLoopMode mode);

//                                                      Source

// sourceo 源结构体定义
typedef struct {
    CFIndex	version;
    void *	info;
    const void *(*retain)(const void *info);
    void	(*release)(const void *info);
    CFStringRef	(*copyDescription)(const void *info);
    Boolean	(*equal)(const void *info1, const void *info2);
    CFHashCode	(*hash)(const void *info);
    void	(*schedule)(void *info, CFRunLoopRef rl, CFRunLoopMode mode);
    void	(*cancel)(void *info, CFRunLoopRef rl, CFRunLoopMode mode);
    void	(*perform)(void *info);
} CFRunLoopSourceContext;

// Source 1 结构体di9ngyi
typedef struct {
    CFIndex	version;
    void *	info;
    const void *(*retain)(const void *info);
    void	(*release)(const void *info);
    CFStringRef	(*copyDescription)(const void *info);
    Boolean	(*equal)(const void *info1, const void *info2);
    CFHashCode	(*hash)(const void *info);
#if (TARGET_OS_MAC && !(TARGET_OS_EMBEDDED || TARGET_OS_IPHONE)) || (TARGET_OS_EMBEDDED || TARGET_OS_IPHONE)
    mach_port_t	(*getPort)(void *info);             // 指定来源 port
    void *	(*perform)(void *msg, CFIndex size, CFAllocatorRef allocator, void *info);
#else
    void *	(*getPort)(void *info);
    void	(*perform)(void *info);
#endif
} CFRunLoopSourceContext1;

CF_EXPORT CFTypeID CFRunLoopSourceGetTypeID(void);

CF_EXPORT CFRunLoopSourceRef CFRunLoopSourceCreate(CFAllocatorRef allocator, CFIndex order, CFRunLoopSourceContext *context);

CF_EXPORT CFIndex CFRunLoopSourceGetOrder(CFRunLoopSourceRef source);
CF_EXPORT void CFRunLoopSourceInvalidate(CFRunLoopSourceRef source);
CF_EXPORT Boolean CFRunLoopSourceIsValid(CFRunLoopSourceRef source);
CF_EXPORT void CFRunLoopSourceGetContext(CFRunLoopSourceRef source, CFRunLoopSourceContext *context);
CF_EXPORT void CFRunLoopSourceSignal(CFRunLoopSourceRef source);


//                                                              Observer
// Observer 结构体定义
typedef struct {
    CFIndex	version;
    void *	info;
    const void *(*retain)(const void *info);
    void	(*release)(const void *info);
    CFStringRef	(*copyDescription)(const void *info);
} CFRunLoopObserverContext;

typedef void (*CFRunLoopObserverCallBack)(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info);

CF_EXPORT CFTypeID CFRunLoopObserverGetTypeID(void);

CF_EXPORT CFRunLoopObserverRef CFRunLoopObserverCreate(CFAllocatorRef allocator, CFOptionFlags activities, Boolean repeats, CFIndex order, CFRunLoopObserverCallBack callout, CFRunLoopObserverContext *context);
#if __BLOCKS__
CF_EXPORT CFRunLoopObserverRef CFRunLoopObserverCreateWithHandler(CFAllocatorRef allocator, CFOptionFlags activities, Boolean repeats, CFIndex order, void (^block) (CFRunLoopObserverRef observer, CFRunLoopActivity activity)) API_AVAILABLE(macos(10.7), ios(5.0), watchos(2.0), tvos(9.0));
#endif

CF_EXPORT CFOptionFlags CFRunLoopObserverGetActivities(CFRunLoopObserverRef observer);
CF_EXPORT Boolean CFRunLoopObserverDoesRepeat(CFRunLoopObserverRef observer);
CF_EXPORT CFIndex CFRunLoopObserverGetOrder(CFRunLoopObserverRef observer);
CF_EXPORT void CFRunLoopObserverInvalidate(CFRunLoopObserverRef observer);
CF_EXPORT Boolean CFRunLoopObserverIsValid(CFRunLoopObserverRef observer);
CF_EXPORT void CFRunLoopObserverGetContext(CFRunLoopObserverRef observer, CFRunLoopObserverContext *context);


///                                                                                                     Timer 部分
// timer 结构体定义
typedef struct {
    CFIndex	version;
    void *	info;
    const void *(*retain)(const void *info);
    void	(*release)(const void *info);
    CFStringRef	(*copyDescription)(const void *info);
} CFRunLoopTimerContext;

typedef void (*CFRunLoopTimerCallBack)(CFRunLoopTimerRef timer, void *info);

CF_EXPORT CFTypeID CFRunLoopTimerGetTypeID(void);

CF_EXPORT CFRunLoopTimerRef CFRunLoopTimerCreate(CFAllocatorRef allocator, CFAbsoluteTime fireDate, CFTimeInterval interval, CFOptionFlags flags, CFIndex order, CFRunLoopTimerCallBack callout, CFRunLoopTimerContext *context);
#if __BLOCKS__
CF_EXPORT CFRunLoopTimerRef CFRunLoopTimerCreateWithHandler(CFAllocatorRef allocator, CFAbsoluteTime fireDate, CFTimeInterval interval, CFOptionFlags flags, CFIndex order, void (^block) (CFRunLoopTimerRef timer)) API_AVAILABLE(macos(10.7), ios(5.0), watchos(2.0), tvos(9.0));
#endif

CF_EXPORT CFAbsoluteTime CFRunLoopTimerGetNextFireDate(CFRunLoopTimerRef timer);
CF_EXPORT void CFRunLoopTimerSetNextFireDate(CFRunLoopTimerRef timer, CFAbsoluteTime fireDate);
CF_EXPORT CFTimeInterval CFRunLoopTimerGetInterval(CFRunLoopTimerRef timer);
CF_EXPORT Boolean CFRunLoopTimerDoesRepeat(CFRunLoopTimerRef timer);
CF_EXPORT CFIndex CFRunLoopTimerGetOrder(CFRunLoopTimerRef timer);
CF_EXPORT void CFRunLoopTimerInvalidate(CFRunLoopTimerRef timer);
CF_EXPORT Boolean CFRunLoopTimerIsValid(CFRunLoopTimerRef timer);
CF_EXPORT void CFRunLoopTimerGetContext(CFRunLoopTimerRef timer, CFRunLoopTimerContext *context);

// Setting a tolerance for a timer allows it to fire later than the scheduled fire date, improving the ability of the system to optimize for increased power savings and responsiveness. The timer may fire at any time between its scheduled fire date and the scheduled fire date plus the tolerance. The timer will not fire before the scheduled fire date. For repeating timers, the next fire date is calculated from the original fire date regardless of tolerance applied at individual fire times, to avoid drift. The default value is zero, which means no additional tolerance is applied. The system reserves the right to apply a small amount of tolerance to certain timers regardless of the value of this property.
// As the user of the timer, you will have the best idea of what an appropriate tolerance for a timer may be. A general rule of thumb, though, is to set the tolerance to at least 10% of the interval, for a repeating timer. Even a small amount of tolerance will have a significant positive impact on the power usage of your application. The system may put a maximum value of the tolerance.
CF_EXPORT CFTimeInterval CFRunLoopTimerGetTolerance(CFRunLoopTimerRef timer) API_AVAILABLE(macos(10.9), ios(7.0), watchos(2.0), tvos(9.0));
CF_EXPORT void CFRunLoopTimerSetTolerance(CFRunLoopTimerRef timer, CFTimeInterval tolerance) API_AVAILABLE(macos(10.9), ios(7.0), watchos(2.0), tvos(9.0));

CF_EXTERN_C_END
CF_IMPLICIT_BRIDGING_DISABLED

#endif /* ! __COREFOUNDATION_CFRUNLOOP__ */

