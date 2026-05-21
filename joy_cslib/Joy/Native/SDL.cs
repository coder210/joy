using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace Livlib.Native
{
    using KeyboardID = UInt32;
    using Keycode = UInt32;
    using Keymod = UInt16;
    using WindowID = UInt32;
    using MouseID = UInt32;
    using MouseButtonFlags = UInt32;
    using JoystickID = UInt32;
    using AudioDeviceID = UInt32;
    using CameraID = UInt32;
    using SensorID = UInt32;
    using TouchID = UInt64;
    using FingerID = UInt64;
    using PenID = UInt32;
    using PenInputFlags = UInt32;

    using Window = IntPtr;
    using Renderer = IntPtr;
    using Texture = IntPtr;


    public struct SDL
    {
        public enum RendererLogicalPresentation
        {
            LOGICAL_PRESENTATION_DISABLED,  /**< There is no logical size in effect */
            LOGICAL_PRESENTATION_STRETCH,   /**< The rendered content is stretched to the output resolution */
            LOGICAL_PRESENTATION_LETTERBOX, /**< The rendered content is fit to the largest dimension and the other dimension is letterboxed with black bars */
            LOGICAL_PRESENTATION_OVERSCAN,  /**< The rendered content is fit to the smallest dimension and the other dimension extends beyond the output bounds */
            LOGICAL_PRESENTATION_INTEGER_SCALE   /**< The rendered content is scaled up by integer multiples to fit the output resolution */
        }


        [StructLayout(LayoutKind.Explicit)]
        public struct Person
        {
            [FieldOffset(0)]
            //private string name;
            private byte[] name;
            [FieldOffset(0)]
            private int age;
            [FieldOffset(0)]
            private float score;

            //public string Name { get { return name; } }
            public byte[] Name { get { return name; } }
            public int Age { get { return age; } }
            public float Score { get { return score; } }
        }


        [StructLayout(LayoutKind.Explicit)]
        public struct Event
        {
            [FieldOffset(0)]
            private EventType type;
            [FieldOffset(0)]
            private CommonEvent common;
            [FieldOffset(0)]
            private KeyboardEvent key;
            [FieldOffset(0)]
            private DisplayEvent display;
            [FieldOffset(0)]
            private WindowEvent window;
            [FieldOffset(0)]
            private KeyboardDeviceEvent kdevice;
            [FieldOffset(0)]
            private TextEditingEvent edit;
            [FieldOffset(0)]
            private TextEditingCandidatesEvent editCandidates;
            [FieldOffset(0)]
            private TextInputEvent text;
            [FieldOffset(0)]
            private MouseDeviceEvent mdevice;
            [FieldOffset(0)]
            private MouseMotionEvent motion;
            [FieldOffset(0)]
            private MouseButtonEvent button;
            [FieldOffset(0)]
            private MouseWheelEvent wheel;
            [FieldOffset(0)]
            private JoyDeviceEvent jdevice;
            [FieldOffset(0)]
            private JoyAxisEvent jaxis;
            [FieldOffset(0)]
            private JoyBallEvent jball;
            [FieldOffset(0)]
            private JoyHatEvent jhat;
            [FieldOffset(0)]
            private JoyButtonEvent jbutton;
            [FieldOffset(0)]
            private JoyBatteryEvent jbattery;
            [FieldOffset(0)]
            private GamepadDeviceEvent gdevice;
            [FieldOffset(0)]
            private GamepadAxisEvent gaxis;
            [FieldOffset(0)]
            private GamepadButtonEvent gbutton;
            [FieldOffset(0)]
            private GamepadTouchpadEvent gtouchpad;
            [FieldOffset(0)]
            private GamepadSensorEvent gsensor;
            [FieldOffset(0)]
            private AudioDeviceEvent adevice;
            [FieldOffset(0)]
            private CameraDeviceEvent cdevice;
            [FieldOffset(0)]
            private SensorEvent sensor;
            [FieldOffset(0)]
            private QuitEvent quit;
            [FieldOffset(0)]
            private UserEvent user;
            [FieldOffset(0)]
            private TouchFingerEvent tfinger;
            [FieldOffset(0)]
            private PenProximityEvent pproximity;
            [FieldOffset(0)]
            private PenTouchEvent ptouch;
            [FieldOffset(0)]
            private PenMotionEvent pmotion;
            [FieldOffset(0)]
            private PenButtonEvent pbutton;
            [FieldOffset(0)]
            private PenAxisEvent paxis;
            [FieldOffset(0)]
            private RenderEvent render;
            [FieldOffset(0)]
            private DropEvent drop;
            [FieldOffset(0)]
            private ClipboardEvent clipboard;
            [FieldOffset(0)]
            private byte[] padding;

            public EventType Type { get { return type; } }
            public CommonEvent Common { get { return common; } }
            public DisplayEvent Display { get { return display; } }
            public WindowEvent Window { get { return window; } }
            public KeyboardDeviceEvent Kdevice { get { return kdevice; } }
            public KeyboardEvent Key { get { return key; } }
            public TextEditingEvent Edit { get { return edit; } }
            public TextEditingCandidatesEvent EditCandidates { get { return editCandidates; } }
            public TextInputEvent Text { get { return text; } }
            public MouseDeviceEvent MDevice { get { return mdevice; } }
            public MouseMotionEvent Motion { get { return motion; } }
            public MouseButtonEvent Button { get { return button; } }
            public MouseWheelEvent Wheel { get { return wheel; } }
            public JoyDeviceEvent JDevice { get { return jdevice; } }
            public JoyAxisEvent JAxis { get { return jaxis; } }
            public JoyBallEvent JBall { get { return jball; } }
            public JoyHatEvent JHat { get { return jhat; } }
            public JoyButtonEvent JButton { get { return jbutton; } }
            public JoyBatteryEvent JBattery { get { return jbattery; } }
            public GamepadDeviceEvent GDevice { get { return gdevice; } }
            public GamepadAxisEvent GAxis { get { return gaxis; } }
            public GamepadButtonEvent GButton { get { return gbutton; } }
            public GamepadTouchpadEvent GTouchpad { get { return gtouchpad; } }
            public GamepadSensorEvent GSensor { get { return gsensor; } }
            public AudioDeviceEvent ADevice { get { return adevice; } }
            public CameraDeviceEvent CDevice { get { return cdevice; } }
            public SensorEvent Sensor { get { return sensor; } }
            public QuitEvent Quit { get { return quit; } }
            public UserEvent User { get { return user; } }
            public TouchFingerEvent TFinger { get { return tfinger; } }
            public PenProximityEvent PProximity { get { return pproximity; } }
            public PenTouchEvent PTouch { get { return ptouch; } }
            public PenMotionEvent PMotion { get { return pmotion; } }
            public PenButtonEvent PButton { get { return pbutton; } }
            public PenAxisEvent PAxis { get { return paxis; } }
            public RenderEvent Render { get { return render; } }
            public DropEvent Drop { get { return drop; } }
            public ClipboardEvent Clipboard { get { return clipboard; } }
            //128
            public byte[] Padding { get { return padding; } }

        }


        public struct WindowsFlag
        {
            public static readonly ulong FULLSCREEN = 0x0000000000000001;
            public static readonly ulong OPENGL = 0x0000000000000002;
            public static readonly ulong OCCLUDED = 0x0000000000000004;
            public static readonly ulong HIDDEN = 0x0000000000000008;
            public static readonly ulong BORDERLESS = 0x0000000000000010;
            public static readonly ulong RESIZABLE = 0x0000000000000020;
            public static readonly ulong MINIMIZED = 0x0000000000000040;
            public static readonly ulong MAXIMIZED = 0x0000000000000080;
            public static readonly ulong MOUSE_GRABBED = 0x0000000000000100;
            public static readonly ulong INPUT_FOCUS = 0x0000000000000200;
            public static readonly ulong MOUSE_FOCUS = 0x0000000000000400;
            public static readonly ulong EXTERNAL = 0x0000000000000800;
            public static readonly ulong MODAL = 0x0000000000001000;
            public static readonly ulong HIGH_PIXEL_DENSITY = 0x0000000000002000;
            public static readonly ulong MOUSE_CAPTURE = 0x0000000000004000;
            public static readonly ulong MOUSE_RELATIVE_MODE = 0x0000000000008000;
            public static readonly ulong ALWAYS_ON_TOP = 0x0000000000010000;
            public static readonly ulong UTILITY = 0x0000000000020000;
            public static readonly ulong TOOLTIP = 0x0000000000040000;
            public static readonly ulong POPUP_MENU = 0x0000000000080000;
            public static readonly ulong KEYBOARD_GRABBED = 0x0000000000100000;
            public static readonly ulong VULKAN = 0x0000000010000000;
            public static readonly ulong METAL = 0x0000000020000000;
            public static readonly ulong TRANSPARENT = 0x0000000040000000;
            public static readonly ulong NOT_FOCUSABLE = 0x0000000080000000;
        }

        public enum EventType
        {
            EVENT_FIRST = 0,     /**< Unused (do not remove) */

            /* Application events */
            EVENT_QUIT = 0x100, /**< User-requested quit */

            /* These application events have special meaning on iOS and Android, see README-ios.md and README-android.md for details */
            EVENT_TERMINATING,      /**< The application is being terminated by the OS. This event must be handled in a callback set with SDL_AddEventWatch().
                                     Called on iOS in applicationWillTerminate()
                                     Called on Android in onDestroy()
                                */
            EVENT_LOW_MEMORY,       /**< The application is low on memory, free memory if possible. This event must be handled in a callback set with SDL_AddEventWatch().
                                     Called on iOS in applicationDidReceiveMemoryWarning()
                                     Called on Android in onTrimMemory()
                                */
            EVENT_WILL_ENTER_BACKGROUND, /**< The application is about to enter the background. This event must be handled in a callback set with SDL_AddEventWatch().
                                     Called on iOS in applicationWillResignActive()
                                     Called on Android in onPause()
                                */
            EVENT_DID_ENTER_BACKGROUND, /**< The application did enter the background and may not get CPU for some time. This event must be handled in a callback set with SDL_AddEventWatch().
                                     Called on iOS in applicationDidEnterBackground()
                                     Called on Android in onPause()
                                */
            EVENT_WILL_ENTER_FOREGROUND, /**< The application is about to enter the foreground. This event must be handled in a callback set with SDL_AddEventWatch().
                                     Called on iOS in applicationWillEnterForeground()
                                     Called on Android in onResume()
                                */
            EVENT_DID_ENTER_FOREGROUND, /**< The application is now interactive. This event must be handled in a callback set with SDL_AddEventWatch().
                                     Called on iOS in applicationDidBecomeActive()
                                     Called on Android in onResume()
                                */

            EVENT_LOCALE_CHANGED,  /**< The user's locale preferences have changed. */

            EVENT_SYSTEM_THEME_CHANGED, /**< The system theme changed */

            /* Display events */
            /* 0x150 was SDL_DISPLAYEVENT, reserve the number for sdl2-compat */
            EVENT_DISPLAY_ORIENTATION = 0x151,   /**< Display orientation has changed to data1 */
            EVENT_DISPLAY_ADDED,                 /**< Display has been added to the system */
            EVENT_DISPLAY_REMOVED,               /**< Display has been removed from the system */
            EVENT_DISPLAY_MOVED,                 /**< Display has changed position */
            EVENT_DISPLAY_DESKTOP_MODE_CHANGED,  /**< Display has changed desktop mode */
            EVENT_DISPLAY_CURRENT_MODE_CHANGED,  /**< Display has changed current mode */
            EVENT_DISPLAY_CONTENT_SCALE_CHANGED, /**< Display has changed content scale */
            EVENT_DISPLAY_FIRST = EVENT_DISPLAY_ORIENTATION,
            EVENT_DISPLAY_LAST = EVENT_DISPLAY_CONTENT_SCALE_CHANGED,

            /* Window events */
            /* 0x200 was SDL_WINDOWEVENT, reserve the number for sdl2-compat */
            /* 0x201 was SDL_SYSWMEVENT, reserve the number for sdl2-compat */
            EVENT_WINDOW_SHOWN = 0x202,     /**< Window has been shown */
            EVENT_WINDOW_HIDDEN,            /**< Window has been hidden */
            EVENT_WINDOW_EXPOSED,           /**< Window has been exposed and should be redrawn, and can be redrawn directly from event watchers for this event */
            EVENT_WINDOW_MOVED,             /**< Window has been moved to data1, data2 */
            EVENT_WINDOW_RESIZED,           /**< Window has been resized to data1xdata2 */
            EVENT_WINDOW_PIXEL_SIZE_CHANGED,/**< The pixel size of the window has changed to data1xdata2 */
            EVENT_WINDOW_METAL_VIEW_RESIZED,/**< The pixel size of a Metal view associated with the window has changed */
            EVENT_WINDOW_MINIMIZED,         /**< Window has been minimized */
            EVENT_WINDOW_MAXIMIZED,         /**< Window has been maximized */
            EVENT_WINDOW_RESTORED,          /**< Window has been restored to normal size and position */
            EVENT_WINDOW_MOUSE_ENTER,       /**< Window has gained mouse focus */
            EVENT_WINDOW_MOUSE_LEAVE,       /**< Window has lost mouse focus */
            EVENT_WINDOW_FOCUS_GAINED,      /**< Window has gained keyboard focus */
            EVENT_WINDOW_FOCUS_LOST,        /**< Window has lost keyboard focus */
            EVENT_WINDOW_CLOSE_REQUESTED,   /**< The window manager requests that the window be closed */
            EVENT_WINDOW_HIT_TEST,          /**< Window had a hit test that wasn't SDL_HITTEST_NORMAL */
            EVENT_WINDOW_ICCPROF_CHANGED,   /**< The ICC profile of the window's display has changed */
            EVENT_WINDOW_DISPLAY_CHANGED,   /**< Window has been moved to display data1 */
            EVENT_WINDOW_DISPLAY_SCALE_CHANGED, /**< Window display scale has been changed */
            EVENT_WINDOW_SAFE_AREA_CHANGED, /**< The window safe area has been changed */
            EVENT_WINDOW_OCCLUDED,          /**< The window has been occluded */
            EVENT_WINDOW_ENTER_FULLSCREEN,  /**< The window has entered fullscreen mode */
            EVENT_WINDOW_LEAVE_FULLSCREEN,  /**< The window has left fullscreen mode */
            EVENT_WINDOW_DESTROYED,         /**< The window with the associated ID is being or has been destroyed. If this message is being handled
                                             in an event watcher, the window handle is still valid and can still be used to retrieve any properties
                                             associated with the window. Otherwise, the handle has already been destroyed and all resources
                                             associated with it are invalid */
            EVENT_WINDOW_HDR_STATE_CHANGED, /**< Window HDR properties have changed */
            EVENT_WINDOW_FIRST = EVENT_WINDOW_SHOWN,
            EVENT_WINDOW_LAST = EVENT_WINDOW_HDR_STATE_CHANGED,

            /* Keyboard events */
            EVENT_KEY_DOWN = 0x300, /**< Key pressed */
            EVENT_KEY_UP,                  /**< Key released */
            EVENT_TEXT_EDITING,            /**< Keyboard text editing (composition) */
            EVENT_TEXT_INPUT,              /**< Keyboard text input */
            EVENT_KEYMAP_CHANGED,          /**< Keymap changed due to a system event such as an
                                            input language or keyboard layout change. */
            EVENT_KEYBOARD_ADDED,          /**< A new keyboard has been inserted into the system */
            EVENT_KEYBOARD_REMOVED,        /**< A keyboard has been removed */
            EVENT_TEXT_EDITING_CANDIDATES, /**< Keyboard text editing candidates */

            /* Mouse events */
            EVENT_MOUSE_MOTION = 0x400, /**< Mouse moved */
            EVENT_MOUSE_BUTTON_DOWN,       /**< Mouse button pressed */
            EVENT_MOUSE_BUTTON_UP,         /**< Mouse button released */
            EVENT_MOUSE_WHEEL,             /**< Mouse wheel motion */
            EVENT_MOUSE_ADDED,             /**< A new mouse has been inserted into the system */
            EVENT_MOUSE_REMOVED,           /**< A mouse has been removed */

            /* Joystick events */
            EVENT_JOYSTICK_AXIS_MOTION = 0x600, /**< Joystick axis motion */
            EVENT_JOYSTICK_BALL_MOTION,          /**< Joystick trackball motion */
            EVENT_JOYSTICK_HAT_MOTION,           /**< Joystick hat position change */
            EVENT_JOYSTICK_BUTTON_DOWN,          /**< Joystick button pressed */
            EVENT_JOYSTICK_BUTTON_UP,            /**< Joystick button released */
            EVENT_JOYSTICK_ADDED,                /**< A new joystick has been inserted into the system */
            EVENT_JOYSTICK_REMOVED,              /**< An opened joystick has been removed */
            EVENT_JOYSTICK_BATTERY_UPDATED,      /**< Joystick battery level change */
            EVENT_JOYSTICK_UPDATE_COMPLETE,      /**< Joystick update is complete */

            /* Gamepad events */
            EVENT_GAMEPAD_AXIS_MOTION = 0x650, /**< Gamepad axis motion */
            EVENT_GAMEPAD_BUTTON_DOWN,          /**< Gamepad button pressed */
            EVENT_GAMEPAD_BUTTON_UP,            /**< Gamepad button released */
            EVENT_GAMEPAD_ADDED,                /**< A new gamepad has been inserted into the system */
            EVENT_GAMEPAD_REMOVED,              /**< A gamepad has been removed */
            EVENT_GAMEPAD_REMAPPED,             /**< The gamepad mapping was updated */
            EVENT_GAMEPAD_TOUCHPAD_DOWN,        /**< Gamepad touchpad was touched */
            EVENT_GAMEPAD_TOUCHPAD_MOTION,      /**< Gamepad touchpad finger was moved */
            EVENT_GAMEPAD_TOUCHPAD_UP,          /**< Gamepad touchpad finger was lifted */
            EVENT_GAMEPAD_SENSOR_UPDATE,        /**< Gamepad sensor was updated */
            EVENT_GAMEPAD_UPDATE_COMPLETE,      /**< Gamepad update is complete */
            EVENT_GAMEPAD_STEAM_HANDLE_UPDATED,  /**< Gamepad Steam handle has changed */

            /* Touch events */
            EVENT_FINGER_DOWN = 0x700,
            EVENT_FINGER_UP,
            EVENT_FINGER_MOTION,
            EVENT_FINGER_CANCELED,

            /* 0x800, 0x801, and 0x802 were the Gesture events from SDL2. Do not reuse these values! sdl2-compat needs them! */

            /* Clipboard events */
            EVENT_CLIPBOARD_UPDATE = 0x900, /**< The clipboard or primary selection changed */

            /* Drag and drop events */
            EVENT_DROP_FILE = 0x1000, /**< The system requests a file open */
            EVENT_DROP_TEXT,                 /**< text/plain drag-and-drop event */
            EVENT_DROP_BEGIN,                /**< A new set of drops is beginning (NULL filename) */
            EVENT_DROP_COMPLETE,             /**< Current set of drops is now complete (NULL filename) */
            EVENT_DROP_POSITION,             /**< Position while moving over the window */

            /* Audio hotplug events */
            EVENT_AUDIO_DEVICE_ADDED = 0x1100,  /**< A new audio device is available */
            EVENT_AUDIO_DEVICE_REMOVED,         /**< An audio device has been removed. */
            EVENT_AUDIO_DEVICE_FORMAT_CHANGED,  /**< An audio device's format has been changed by the system. */

            /* Sensor events */
            EVENT_SENSOR_UPDATE = 0x1200,     /**< A sensor was updated */

            /* Pressure-sensitive pen events */
            EVENT_PEN_PROXIMITY_IN = 0x1300,  /**< Pressure-sensitive pen has become available */
            EVENT_PEN_PROXIMITY_OUT,          /**< Pressure-sensitive pen has become unavailable */
            EVENT_PEN_DOWN,                   /**< Pressure-sensitive pen touched drawing surface */
            EVENT_PEN_UP,                     /**< Pressure-sensitive pen stopped touching drawing surface */
            EVENT_PEN_BUTTON_DOWN,            /**< Pressure-sensitive pen button pressed */
            EVENT_PEN_BUTTON_UP,              /**< Pressure-sensitive pen button released */
            EVENT_PEN_MOTION,                 /**< Pressure-sensitive pen is moving on the tablet */
            EVENT_PEN_AXIS,                   /**< Pressure-sensitive pen angle/pressure/etc changed */

            /* Camera hotplug events */
            EVENT_CAMERA_DEVICE_ADDED = 0x1400,  /**< A new camera device is available */
            EVENT_CAMERA_DEVICE_REMOVED,         /**< A camera device has been removed. */
            EVENT_CAMERA_DEVICE_APPROVED,        /**< A camera device has been approved for use by the user. */
            EVENT_CAMERA_DEVICE_DENIED,          /**< A camera device has been denied for use by the user. */

            /* Render events */
            EVENT_RENDER_TARGETS_RESET = 0x2000, /**< The render targets have been reset and their contents need to be updated */
            EVENT_RENDER_DEVICE_RESET, /**< The device has been reset and all textures need to be recreated */
            EVENT_RENDER_DEVICE_LOST, /**< The device has been lost and can't be recovered. */

            /* Reserved events for private platforms */
            EVENT_PRIVATE0 = 0x4000,
            EVENT_PRIVATE1,
            EVENT_PRIVATE2,
            EVENT_PRIVATE3,

            /* Internal events */
            EVENT_POLL_SENTINEL = 0x7F00, /**< Signals the end of an event poll cycle */

            /** Events EVENT_USER through EVENT_LAST are for your use,
             *  and should be allocated with SDL_RegisterEvents()
             */
            EVENT_USER = 0x8000,

            /**
             *  This last event is only for bounding internal arrays
             */
            EVENT_LAST = 0xFFFF,

            /* This just makes sure the enum is the size of Uint32 */
            EVENT_ENUM_PADDING = 0x7FFFFFFF

        }

        public struct CommonEvent
        {
            public uint Type { get; set; }
            public uint Reserved { get; set; }
            public ulong Timestamp { get; set; }
        }

        public struct DisplayEvent
        {
            public EventType Type { get; set; }
            public uint Reserved { get; set; }
            public ulong Timestamp { get; set; }
            public uint DisplayID { get; set; }
            public int Data1 { get; set; }
            public int Data2 { get; set; }
        }

        public struct WindowEvent
        {
            public EventType Type { get; set; }
            public uint Reserved { get; set; }
            public ulong Timestamp { get; set; }
            public uint WindowID { get; set; }
            public int Data1 { get; set; }
            public int Data2 { get; set; }
        }

        public struct KeyboardDeviceEvent
        {
            public EventType Type { get; set; }
            public uint Reserved { get; set; }
            public ulong Timestamp { get; set; }
            public KeyboardID Which { get; set; }
        }

        public struct KeyboardEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public UInt32 WindowID { get; set; }
            public KeyboardID Which { get; set; }
            public Scancode Scancode { get; set; }
            public Keycode Key { get; set; }
            public Keymod Mod { get; set; }
            public UInt16 Raw { get; set; }
            public bool Down { get; set; }
            public bool Repeat { get; set; }
        }

        public struct TextEditingEvent
        {
            public EventType Type { get; set; }
            public uint Reserved { get; set; }
            public ulong Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public byte[] Text { get; set; }
            public int Start { get; set; }
            public int Length { get; set; }
        }

        public struct TextEditingCandidatesEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public byte[] Candidates { get; set; }
            public int NumCandidates { get; set; }
            public int SelectedCandidate { get; set; }
            public bool Horizontal { get; set; }
            public byte Padding1 { get; set; }
            public byte Padding2 { get; set; }
            public byte Padding3 { get; set; }
        }

        public struct TextInputEvent
        {
            public EventType Type { get; set; }
            public uint Reserved { get; set; }
            public ulong Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public byte[] Text { get; set; }
        }

        public struct MouseDeviceEvent
        {
            public EventType Type { get; set; }
            public uint Reserved { get; set; }
            public ulong Timestamp { get; set; }
            public MouseID Which { get; set; }
        }

        public struct MouseMotionEvent
        {
            public EventType Type { get; set; }
            public uint Reserved { get; set; }
            public uint Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public MouseID Which { get; set; }
            public MouseButtonFlags State { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
            public float XRel { get; set; }
            public float YRel { get; set; }
        }

        public struct MouseButtonEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public MouseID Which { get; set; }
            public byte Button { get; set; }
            public bool Down { get; set; }
            public byte Clicks { get; set; }
            public byte Padding { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
        }

        public enum MouseWheelDirection
        {
            SDL_MOUSEWHEEL_NORMAL,
            SDL_MOUSEWHEEL_FLIPPED
        }

        public struct MouseWheelEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public MouseID Which { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
            public MouseWheelDirection Direction { get; set; }
            public float MouseX { get; set; }
            public float MouseY { get; set; }
            public int IntegerX { get; set; }
            public int IntegerY { get; set; }
        }

        public struct JoyDeviceEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
        }

        public struct JoyAxisEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public byte Axis { get; set; }
            public byte Padding1 { get; set; }
            public byte Padding2 { get; set; }
            public byte Padding3 { get; set; }
            public short Value { get; set; }
            public short Padding4 { get; set; }
        }

        public struct JoyBallEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public byte Ball { get; set; }
            public byte Padding1 { get; set; }
            public byte Padding2 { get; set; }
            public byte Padding3 { get; set; }
            public short XRel { get; set; }
            public short YRel { get; set; }
        }

        public struct JoyHatEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public byte Hat { get; set; }
            public byte Value { get; set; }
            public byte Padding1 { get; set; }
            public byte Padding2 { get; set; }
        }

        public struct JoyButtonEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public byte Button { get; set; }
            public bool Down { get; set; }
            public byte Padding1 { get; set; }
            public byte Padding2 { get; set; }
        }

        public enum PowerState
        {
            SDL_POWERSTATE_ERROR = -1,
            SDL_POWERSTATE_UNKNOWN,
            SDL_POWERSTATE_ON_BATTERY,
            SDL_POWERSTATE_NO_BATTERY,
            SDL_POWERSTATE_CHARGING,
            SDL_POWERSTATE_CHARGED
        }

        public struct JoyBatteryEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public PowerState State { get; set; }
            public int Percent { get; set; }
        }

        public struct GamepadDeviceEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
        }

        public struct GamepadAxisEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public byte Axis { get; set; }
            public byte Padding1 { get; set; }
            public byte Padding2 { get; set; }
            public byte Padding3 { get; set; }
            public short Value { get; set; }
            public short Padding4 { get; set; }
        }

        public struct GamepadButtonEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public byte Button { get; set; }
            public bool Down { get; set; }
            public byte Padding1 { get; set; }
            public byte Padding2 { get; set; }
        }

        public struct GamepadTouchpadEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public int Touchpad { get; set; }
            public int Finger { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
            public float Pressure { get; set; }
        }

        public struct GamepadSensorEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public JoystickID Which { get; set; }
            public int Sensor { get; set; }
            public float[] Data { get; set; }//3
            public UInt64 SensorTimestamp { get; set; }
        }

        public struct AudioDeviceEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public AudioDeviceID Which { get; set; }
            public bool Recording { get; set; }
            public byte Padding1 { get; set; }
            public byte Padding2 { get; set; }
            public byte Padding3 { get; set; }
        }

        public struct CameraDeviceEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public CameraID Which { get; set; }
        }

        public struct SensorEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public SensorID Which { get; set; }
            // 6
            public float[] Data { get; set; }
            public UInt64 SensorTimestamp { get; set; }
        }

        public struct QuitEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
        }

        public struct UserEvent
        {
            public UInt32 Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public int Code { get; set; }
            public IntPtr Data1 { get; set; }
            public IntPtr Data2 { get; set; }
        }

        public struct TouchFingerEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public TouchID TouchID { get; set; }
            public FingerID FingerID { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
            public float DX { get; set; }
            public float DY { get; set; }
            public float Pressure { get; set; }
            public WindowID WindowID { get; set; }
        }

        public struct PenProximityEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public PenID Which { get; set; }
        }

        public struct PenTouchEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public PenID Which { get; set; }
            public PenInputFlags PenState { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
            public bool Eraser { get; set; }
            public bool Down { get; set; }
        }

        public struct PenMotionEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public PenID Which { get; set; }
            public PenInputFlags PenState { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
        }

        public struct PenButtonEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public PenID Which { get; set; }
            public PenInputFlags PenState { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
            public byte Button { get; set; }
            public bool Down { get; set; }
        }

        public enum PenAxis
        {
            SDL_PEN_AXIS_PRESSURE,  /**< Pen pressure.  Unidirectional: 0 to 1.0 */
            SDL_PEN_AXIS_XTILT,     /**< Pen horizontal tilt angle.  Bidirectional: -90.0 to 90.0 (left-to-right). */
            SDL_PEN_AXIS_YTILT,     /**< Pen vertical tilt angle.  Bidirectional: -90.0 to 90.0 (top-to-down). */
            SDL_PEN_AXIS_DISTANCE,  /**< Pen distance to drawing surface.  Unidirectional: 0.0 to 1.0 */
            SDL_PEN_AXIS_ROTATION,  /**< Pen barrel rotation.  Bidirectional: -180 to 179.9 (clockwise, 0 is facing up, -180.0 is facing down). */
            SDL_PEN_AXIS_SLIDER,    /**< Pen finger wheel or slider (e.g., Airbrush Pen).  Unidirectional: 0 to 1.0 */
            SDL_PEN_AXIS_TANGENTIAL_PRESSURE,    /**< Pressure from squeezing the pen ("barrel pressure"). */
            SDL_PEN_AXIS_COUNT       /**< Total known pen axis types in this version of SDL. This number may grow in future releases! */
        }

        public struct PenAxisEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public PenID Which { get; set; }
            public PenInputFlags PenState { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
            public PenAxis Axis { get; set; }
            public float Value { get; set; }
        }

        public struct RenderEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
        }

        public struct DropEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public WindowID WindowID { get; set; }
            public float X { get; set; }
            public float Y { get; set; }
            public byte[] Source { get; set; }
            public byte[] Data { get; set; }
        }
        public struct ClipboardEvent
        {
            public EventType Type { get; set; }
            public UInt32 Reserved { get; set; }
            public UInt64 Timestamp { get; set; }
            public bool Owner { get; set; }
            public int NumMimeTypes { get; set; }
            public byte[][] MimeTypes { get; set; }
        }


        public enum Scancode
        {
            SCANCODE_UNKNOWN = 0,

            /**
             *  \name Usage page 0x07
             *
             *  These values are from usage page 0x07 (USB keyboard page).
             */
            /* @{ */

            SCANCODE_A = 4,
            SCANCODE_B = 5,
            SCANCODE_C = 6,
            SCANCODE_D = 7,
            SCANCODE_E = 8,
            SCANCODE_F = 9,
            SCANCODE_G = 10,
            SCANCODE_H = 11,
            SCANCODE_I = 12,
            SCANCODE_J = 13,
            SCANCODE_K = 14,
            SCANCODE_L = 15,
            SCANCODE_M = 16,
            SCANCODE_N = 17,
            SCANCODE_O = 18,
            SCANCODE_P = 19,
            SCANCODE_Q = 20,
            SCANCODE_R = 21,
            SCANCODE_S = 22,
            SCANCODE_T = 23,
            SCANCODE_U = 24,
            SCANCODE_V = 25,
            SCANCODE_W = 26,
            SCANCODE_X = 27,
            SCANCODE_Y = 28,
            SCANCODE_Z = 29,

            SCANCODE_1 = 30,
            SCANCODE_2 = 31,
            SCANCODE_3 = 32,
            SCANCODE_4 = 33,
            SCANCODE_5 = 34,
            SCANCODE_6 = 35,
            SCANCODE_7 = 36,
            SCANCODE_8 = 37,
            SCANCODE_9 = 38,
            SCANCODE_0 = 39,

            SCANCODE_RETURN = 40,
            SCANCODE_ESCAPE = 41,
            SCANCODE_BACKSPACE = 42,
            SCANCODE_TAB = 43,
            SCANCODE_SPACE = 44,

            SCANCODE_MINUS = 45,
            SCANCODE_EQUALS = 46,
            SCANCODE_LEFTBRACKET = 47,
            SCANCODE_RIGHTBRACKET = 48,
            SCANCODE_BACKSLASH = 49, /**< Located at the lower left of the return
                                  *   key on ISO keyboards and at the right end
                                  *   of the QWERTY row on ANSI keyboards.
                                  *   Produces REVERSE SOLIDUS (backslash) and
                                  *   VERTICAL LINE in a US layout, REVERSE
                                  *   SOLIDUS and VERTICAL LINE in a UK Mac
                                  *   layout, NUMBER SIGN and TILDE in a UK
                                  *   Windows layout, DOLLAR SIGN and POUND SIGN
                                  *   in a Swiss German layout, NUMBER SIGN and
                                  *   APOSTROPHE in a German layout, GRAVE
                                  *   ACCENT and POUND SIGN in a French Mac
                                  *   layout, and ASTERISK and MICRO SIGN in a
                                  *   French Windows layout.
                                  */
            SCANCODE_NONUSHASH = 50, /**< ISO USB keyboards actually use this code
                                  *   instead of 49 for the same key, but all
                                  *   OSes I've seen treat the two codes
                                  *   identically. So, as an implementor, unless
                                  *   your keyboard generates both of those
                                  *   codes and your OS treats them differently,
                                  *   you should generate SCANCODE_BACKSLASH
                                  *   instead of this code. As a user, you
                                  *   should not rely on this code because SDL
                                  *   will never generate it with most (all?)
                                  *   keyboards.
                                  */
            SCANCODE_SEMICOLON = 51,
            SCANCODE_APOSTROPHE = 52,
            SCANCODE_GRAVE = 53, /**< Located in the top left corner (on both ANSI
                              *   and ISO keyboards). Produces GRAVE ACCENT and
                              *   TILDE in a US Windows layout and in US and UK
                              *   Mac layouts on ANSI keyboards, GRAVE ACCENT
                              *   and NOT SIGN in a UK Windows layout, SECTION
                              *   SIGN and PLUS-MINUS SIGN in US and UK Mac
                              *   layouts on ISO keyboards, SECTION SIGN and
                              *   DEGREE SIGN in a Swiss German layout (Mac:
                              *   only on ISO keyboards), CIRCUMFLEX ACCENT and
                              *   DEGREE SIGN in a German layout (Mac: only on
                              *   ISO keyboards), SUPERSCRIPT TWO and TILDE in a
                              *   French Windows layout, COMMERCIAL AT and
                              *   NUMBER SIGN in a French Mac layout on ISO
                              *   keyboards, and LESS-THAN SIGN and GREATER-THAN
                              *   SIGN in a Swiss German, German, or French Mac
                              *   layout on ANSI keyboards.
                              */
            SCANCODE_COMMA = 54,
            SCANCODE_PERIOD = 55,
            SCANCODE_SLASH = 56,

            SCANCODE_CAPSLOCK = 57,

            SCANCODE_F1 = 58,
            SCANCODE_F2 = 59,
            SCANCODE_F3 = 60,
            SCANCODE_F4 = 61,
            SCANCODE_F5 = 62,
            SCANCODE_F6 = 63,
            SCANCODE_F7 = 64,
            SCANCODE_F8 = 65,
            SCANCODE_F9 = 66,
            SCANCODE_F10 = 67,
            SCANCODE_F11 = 68,
            SCANCODE_F12 = 69,

            SCANCODE_PRINTSCREEN = 70,
            SCANCODE_SCROLLLOCK = 71,
            SCANCODE_PAUSE = 72,
            SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but
                                   does send code 73, not 117) */
            SCANCODE_HOME = 74,
            SCANCODE_PAGEUP = 75,
            SCANCODE_DELETE = 76,
            SCANCODE_END = 77,
            SCANCODE_PAGEDOWN = 78,
            SCANCODE_RIGHT = 79,
            SCANCODE_LEFT = 80,
            SCANCODE_DOWN = 81,
            SCANCODE_UP = 82,

            SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards
                                     */
            SCANCODE_KP_DIVIDE = 84,
            SCANCODE_KP_MULTIPLY = 85,
            SCANCODE_KP_MINUS = 86,
            SCANCODE_KP_PLUS = 87,
            SCANCODE_KP_ENTER = 88,
            SCANCODE_KP_1 = 89,
            SCANCODE_KP_2 = 90,
            SCANCODE_KP_3 = 91,
            SCANCODE_KP_4 = 92,
            SCANCODE_KP_5 = 93,
            SCANCODE_KP_6 = 94,
            SCANCODE_KP_7 = 95,
            SCANCODE_KP_8 = 96,
            SCANCODE_KP_9 = 97,
            SCANCODE_KP_0 = 98,
            SCANCODE_KP_PERIOD = 99,

            SCANCODE_NONUSBACKSLASH = 100, /**< This is the additional key that ISO
                                        *   keyboards have over ANSI ones,
                                        *   located between left shift and Y.
                                        *   Produces GRAVE ACCENT and TILDE in a
                                        *   US or UK Mac layout, REVERSE SOLIDUS
                                        *   (backslash) and VERTICAL LINE in a
                                        *   US or UK Windows layout, and
                                        *   LESS-THAN SIGN and GREATER-THAN SIGN
                                        *   in a Swiss German, German, or French
                                        *   layout. */
            SCANCODE_APPLICATION = 101, /**< windows contextual menu, compose */
            SCANCODE_POWER = 102, /**< The USB document says this is a status flag,
                               *   not a physical key - but some Mac keyboards
                               *   do have a power key. */
            SCANCODE_KP_EQUALS = 103,
            SCANCODE_F13 = 104,
            SCANCODE_F14 = 105,
            SCANCODE_F15 = 106,
            SCANCODE_F16 = 107,
            SCANCODE_F17 = 108,
            SCANCODE_F18 = 109,
            SCANCODE_F19 = 110,
            SCANCODE_F20 = 111,
            SCANCODE_F21 = 112,
            SCANCODE_F22 = 113,
            SCANCODE_F23 = 114,
            SCANCODE_F24 = 115,
            SCANCODE_EXECUTE = 116,
            SCANCODE_HELP = 117,    /**< AL Integrated Help Center */
            SCANCODE_MENU = 118,    /**< Menu (show menu) */
            SCANCODE_SELECT = 119,
            SCANCODE_STOP = 120,    /**< AC Stop */
            SCANCODE_AGAIN = 121,   /**< AC Redo/Repeat */
            SCANCODE_UNDO = 122,    /**< AC Undo */
            SCANCODE_CUT = 123,     /**< AC Cut */
            SCANCODE_COPY = 124,    /**< AC Copy */
            SCANCODE_PASTE = 125,   /**< AC Paste */
            SCANCODE_FIND = 126,    /**< AC Find */
            SCANCODE_MUTE = 127,
            SCANCODE_VOLUMEUP = 128,
            SCANCODE_VOLUMEDOWN = 129,
            /* not sure whether there's a reason to enable these */
            /*     SCANCODE_LOCKINGCAPSLOCK = 130,  */
            /*     SCANCODE_LOCKINGNUMLOCK = 131, */
            /*     SCANCODE_LOCKINGSCROLLLOCK = 132, */
            SCANCODE_KP_COMMA = 133,
            SCANCODE_KP_EQUALSAS400 = 134,

            SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see
                                            footnotes in USB doc */
            SCANCODE_INTERNATIONAL2 = 136,
            SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
            SCANCODE_INTERNATIONAL4 = 138,
            SCANCODE_INTERNATIONAL5 = 139,
            SCANCODE_INTERNATIONAL6 = 140,
            SCANCODE_INTERNATIONAL7 = 141,
            SCANCODE_INTERNATIONAL8 = 142,
            SCANCODE_INTERNATIONAL9 = 143,
            SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
            SCANCODE_LANG2 = 145, /**< Hanja conversion */
            SCANCODE_LANG3 = 146, /**< Katakana */
            SCANCODE_LANG4 = 147, /**< Hiragana */
            SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
            SCANCODE_LANG6 = 149, /**< reserved */
            SCANCODE_LANG7 = 150, /**< reserved */
            SCANCODE_LANG8 = 151, /**< reserved */
            SCANCODE_LANG9 = 152, /**< reserved */

            SCANCODE_ALTERASE = 153,    /**< Erase-Eaze */
            SCANCODE_SYSREQ = 154,
            SCANCODE_CANCEL = 155,      /**< AC Cancel */
            SCANCODE_CLEAR = 156,
            SCANCODE_PRIOR = 157,
            SCANCODE_RETURN2 = 158,
            SCANCODE_SEPARATOR = 159,
            SCANCODE_OUT = 160,
            SCANCODE_OPER = 161,
            SCANCODE_CLEARAGAIN = 162,
            SCANCODE_CRSEL = 163,
            SCANCODE_EXSEL = 164,

            SCANCODE_KP_00 = 176,
            SCANCODE_KP_000 = 177,
            SCANCODE_THOUSANDSSEPARATOR = 178,
            SCANCODE_DECIMALSEPARATOR = 179,
            SCANCODE_CURRENCYUNIT = 180,
            SCANCODE_CURRENCYSUBUNIT = 181,
            SCANCODE_KP_LEFTPAREN = 182,
            SCANCODE_KP_RIGHTPAREN = 183,
            SCANCODE_KP_LEFTBRACE = 184,
            SCANCODE_KP_RIGHTBRACE = 185,
            SCANCODE_KP_TAB = 186,
            SCANCODE_KP_BACKSPACE = 187,
            SCANCODE_KP_A = 188,
            SCANCODE_KP_B = 189,
            SCANCODE_KP_C = 190,
            SCANCODE_KP_D = 191,
            SCANCODE_KP_E = 192,
            SCANCODE_KP_F = 193,
            SCANCODE_KP_XOR = 194,
            SCANCODE_KP_POWER = 195,
            SCANCODE_KP_PERCENT = 196,
            SCANCODE_KP_LESS = 197,
            SCANCODE_KP_GREATER = 198,
            SCANCODE_KP_AMPERSAND = 199,
            SCANCODE_KP_DBLAMPERSAND = 200,
            SCANCODE_KP_VERTICALBAR = 201,
            SCANCODE_KP_DBLVERTICALBAR = 202,
            SCANCODE_KP_COLON = 203,
            SCANCODE_KP_HASH = 204,
            SCANCODE_KP_SPACE = 205,
            SCANCODE_KP_AT = 206,
            SCANCODE_KP_EXCLAM = 207,
            SCANCODE_KP_MEMSTORE = 208,
            SCANCODE_KP_MEMRECALL = 209,
            SCANCODE_KP_MEMCLEAR = 210,
            SCANCODE_KP_MEMADD = 211,
            SCANCODE_KP_MEMSUBTRACT = 212,
            SCANCODE_KP_MEMMULTIPLY = 213,
            SCANCODE_KP_MEMDIVIDE = 214,
            SCANCODE_KP_PLUSMINUS = 215,
            SCANCODE_KP_CLEAR = 216,
            SCANCODE_KP_CLEARENTRY = 217,
            SCANCODE_KP_BINARY = 218,
            SCANCODE_KP_OCTAL = 219,
            SCANCODE_KP_DECIMAL = 220,
            SCANCODE_KP_HEXADECIMAL = 221,

            SCANCODE_LCTRL = 224,
            SCANCODE_LSHIFT = 225,
            SCANCODE_LALT = 226, /**< alt, option */
            SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
            SCANCODE_RCTRL = 228,
            SCANCODE_RSHIFT = 229,
            SCANCODE_RALT = 230, /**< alt gr, option */
            SCANCODE_RGUI = 231, /**< windows, command (apple), meta */

            SCANCODE_MODE = 257,    /**< I'm not sure if this is really not covered
                                 *   by any of the above, but since there's a
                                 *   special KMOD_MODE for it I'm adding it here
                                 */

            /* @} *//* Usage page 0x07 */

            /**
             *  \name Usage page 0x0C
             *
             *  These values are mapped from usage page 0x0C (USB consumer page).
             *
             *  There are way more keys in the spec than we can represent in the
             *  current scancode range, so pick the ones that commonly come up in
             *  real world usage.
             */
            /* @{ */

            SCANCODE_SLEEP = 258,                   /**< Sleep */
            SCANCODE_WAKE = 259,                    /**< Wake */

            SCANCODE_CHANNEL_INCREMENT = 260,       /**< Channel Increment */
            SCANCODE_CHANNEL_DECREMENT = 261,       /**< Channel Decrement */

            SCANCODE_MEDIA_PLAY = 262,          /**< Play */
            SCANCODE_MEDIA_PAUSE = 263,         /**< Pause */
            SCANCODE_MEDIA_RECORD = 264,        /**< Record */
            SCANCODE_MEDIA_FAST_FORWARD = 265,  /**< Fast Forward */
            SCANCODE_MEDIA_REWIND = 266,        /**< Rewind */
            SCANCODE_MEDIA_NEXT_TRACK = 267,    /**< Next Track */
            SCANCODE_MEDIA_PREVIOUS_TRACK = 268, /**< Previous Track */
            SCANCODE_MEDIA_STOP = 269,          /**< Stop */
            SCANCODE_MEDIA_EJECT = 270,         /**< Eject */
            SCANCODE_MEDIA_PLAY_PAUSE = 271,    /**< Play / Pause */
            SCANCODE_MEDIA_SELECT = 272,        /* Media Select */

            SCANCODE_AC_NEW = 273,              /**< AC New */
            SCANCODE_AC_OPEN = 274,             /**< AC Open */
            SCANCODE_AC_CLOSE = 275,            /**< AC Close */
            SCANCODE_AC_EXIT = 276,             /**< AC Exit */
            SCANCODE_AC_SAVE = 277,             /**< AC Save */
            SCANCODE_AC_PRINT = 278,            /**< AC Print */
            SCANCODE_AC_PROPERTIES = 279,       /**< AC Properties */

            SCANCODE_AC_SEARCH = 280,           /**< AC Search */
            SCANCODE_AC_HOME = 281,             /**< AC Home */
            SCANCODE_AC_BACK = 282,             /**< AC Back */
            SCANCODE_AC_FORWARD = 283,          /**< AC Forward */
            SCANCODE_AC_STOP = 284,             /**< AC Stop */
            SCANCODE_AC_REFRESH = 285,          /**< AC Refresh */
            SCANCODE_AC_BOOKMARKS = 286,        /**< AC Bookmarks */

            /* @} *//* Usage page 0x0C */


            /**
             *  \name Mobile keys
             *
             *  These are values that are often used on mobile phones.
             */
            /* @{ */

            SCANCODE_SOFTLEFT = 287, /**< Usually situated below the display on phones and
                                      used as a multi-function feature key for selecting
                                      a software defined function shown on the bottom left
                                      of the display. */
            SCANCODE_SOFTRIGHT = 288, /**< Usually situated below the display on phones and
                                       used as a multi-function feature key for selecting
                                       a software defined function shown on the bottom right
                                       of the display. */
            SCANCODE_CALL = 289, /**< Used for accepting phone calls. */
            SCANCODE_ENDCALL = 290, /**< Used for rejecting phone calls. */

            /* @} *//* Mobile keys */

            /* Add any other keys here. */

            SCANCODE_RESERVED = 400,    /**< 400-500 reserved for dynamic keycodes */

            SCANCODE_COUNT = 512 /**< not a key, just marks the number of scancodes for array bounds */

        }

        public struct FRect
        {
            public float X { get; set; }
            public float Y { get; set; }
            public float W { get; set; }
            public float H { get; set; }
        }

        public struct FPoint
        {
            public float X { get; set; }
            public float Y { get; set; }
        }

        public struct FColor
        {
            public float R { get; set; }
            public float G { get; set; }
            public float B { get; set; }
            public float A { get; set; }
        }

        public enum FlipMode
        {
            FLIP_NONE,
            FLIP_HORIZONTAL,
            FLIP_VERTICAL
        }

        /// <summary>
        /// bool SDLCALL SetAppMetadata(const char *appname, const char *appversion, const char *appidentifier);
        /// </summary>
        /// <param name="appname"></param>
        /// <param name="appversion"></param>
        /// <param name="appidentifier"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetAppMetadata(string appname, string appversion, string appidentifier);

        /// <summary>
        /// DECLSPEC bool SDLCALL Init(InitFlags flags);
        /// </summary>
        /// <param name="flags"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool Init(int flags);

        /// <summary>
        /// void SDLCALL Quit(void);
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool Quit();

        /// <summary>
        /// extern DECLSPEC int SDLCALL GetVersion(void);
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int GetVersion();

        /// <summary>
        ///void SDLCALL Log(PRINTF_FORMAT_STRING const char *fmt, ...) PRINTF_VARARG_FUNC(1)
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Log(string fmt, string str);

        /// <summary>
        ///DECLSPEC void SDLCALL LogError(int category, PRINTF_FORMAT_STRING const char *fmt, ...) PRINTF_VARARG_FUNC(2);
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void LogError(int category, string fmt, string str);

        /// <summary>
        /// DECLSPEC void SDLCALL Delay(Uint32 ms)
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void Delay(UInt32 ms);

        /// <summary>
        /// DisplayMode * SDLCALL GetCurrentDisplayMode(DisplayID displayID)
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr GetCurrentDisplayMode(UInt32 displayID);

        /// <summary>
        ///  Window * SDLCALL CreateWindow(const char* title, int w, int h, WindowFlags flags)
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Window CreateWindow(string title, int w, int h, UInt64 flags);

        /// <summary>
        /// bool SDLCALL GetWindowSize(Window *window, int *w, int *h);
        /// </summary>
        /// <param name="title"></param>
        /// <param name="w"></param>
        /// <param name="h"></param>
        /// <param name="flags"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GetWindowSize(Window window, int w, int h);

        /// <summary>
        /// void SDLCALL DestroyWindow(Window *window);
        /// </summary>
        /// <param name="window"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void DestroyWindow(Window window);

        /// <summary>
        /// bool SDLCALL CreateWindowAndRenderer(const char *title, int width, int height, WindowFlags window_flags, Window **window, Renderer **renderer);
        /// </summary>
        /// <param name="window"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool CreateWindowAndRenderer(string title, int width, int height, ulong windowFlags, out Window window, out Renderer renderer);

        /// <summary>
        /// int set_window_icon(Window* window, const char* bmp)
        /// </summary>
        /// <param name="renderer"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetWindowIcon(Window window, string bmp);

        /// <summary>
        /// DECLSPEC bool SDLCALL SetWindowFullscreen(Window *window, bool fullscreen);
        /// </summary>
        /// <param name="window"></param>
        /// <param name="fullscreen"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetWindowFullscreen(Window window, bool fullscreen);

        /// <summary>
        /// DECLSPEC Renderer * SDLCALL CreateRenderer(Window *window, const char *name);
        /// </summary>
        /// <param name="window"></param>
        /// <param name="name"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool CreateRenderer(Window window, string name);

        /// <summary>
        /// extern DECLSPEC void SDLCALL DestroyRenderer(Renderer *renderer);
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void DestroyRenderer(Renderer renderer);

        /// <summary>
        /// extern DECLSPEC bool SDLCALL SetRenderVSync(Renderer *renderer, int vsync);
        /// </summary>
        /// <param name="renderer"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetRenderVSync(Renderer renderer, int vsync);


        /// <summary>
        /// bool SDLCALL RenderClear(Renderer *renderer);
        /// </summary>
        /// <param name="renderer"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RenderClear(Renderer renderer);

        /// <summary>
        /// bool SDLCALL RenderPresent(Renderer *renderer);
        /// </summary>
        /// <param name="renderer"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RenderPresent(Renderer renderer);

        /// <summary>
        /// bool SDLCALL SetRenderScale(Renderer* renderer, float scaleX, float scaleY);
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="scaleX"></param>
        /// <param name="scaleY"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetRenderScale(Renderer renderer, float scaleX, float scaleY);

        /// <summary>
        /// bool SDLCALL RenderDebugText(Renderer *renderer, float x, float y, const char *str);
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <param name="str"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RenderDebugText(Renderer renderer, float x, float y, string str);

        /// <summary>
        /// bool SDLCALL SetRenderDrawColor(Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="r"></param>
        /// <param name="g"></param>
        /// <param name="b"></param>
        /// <param name="a"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetRenderDrawColor(Renderer renderer, byte r, byte g, byte b, byte a);

        /// <summary>
        /// bool SDLCALL RenderPoint(Renderer *renderer, float x, float y);
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="x"></param>
        /// <param name="y"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RenderPoint(Renderer renderer, float x, float y);

        /// <summary>
        /// bool SDLCALL RenderLine(Renderer *renderer, float x1, float y1, float x2, float y2);
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="x1"></param>
        /// <param name="y1"></param>
        /// <param name="x2"></param>
        /// <param name="y2"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RenderLine(Renderer renderer, float x1, float y1, float x2, float y2);

        /// <summary>
        /// bool SDLCALL RenderRect(Renderer *renderer, const FRect *rect);
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="rect"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RenderRect(Renderer renderer, FRect rect);

        /// <summary>
        /// bool SDLCALL RenderFillRect(Renderer *renderer, const FRect *rect);
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="rect"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RenderFillRect(Renderer renderer, FRect rect);

        /// <summary>
        /// static stbtt_fontinfo* open_font(const char* filename)
        /// </summary>
        /// <param name="renderer"></param>
        /// <param name="rect"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr OpenFont(string filename);

        /// <summary>
        /// static void close_font(stbtt_fontinfo* font_info)
        /// </summary>
        /// <param name="fontInfo"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CloseFont(IntPtr fontInfo);

        /// <summary>
        /// bool SDLCALL LoadWAV(const char* path, AudioSpec *spec, Uint8** audio_buf, Uint32 *audio_len);
        /// </summary>
        /// <param name="path"></param>
        /// <param name="spec"></param>
        /// <param name="audioBuf"></param>
        /// <param name="audioLen"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool LoadWAV(string path, IntPtr spec, out byte audioBuf, out uint audioLen);

        /// <summary>
        /// AudioStream * SDLCALL CreateAudioStream(const AudioSpec* src_spec, const AudioSpec* dst_spec); 
        /// </summary>
        /// <param name="srcAudioSpec"></param>
        /// <param name="destAudioSpec"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateAudioStream(IntPtr srcAudioSpec, IntPtr destAudioSpec);

        /// <summary>
        /// static AudioDeviceID open_default_audio_device()
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static uint OpenDefaultAudioDevice();

        /// <summary>
        ///  bool SDLCALL PauseAudioDevice(AudioDeviceID devid);
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool PauseAudioDevice(uint devid);

        /// <summary>
        /// bool SDLCALL ResumeAudioDevice(AudioDeviceID devid);
        /// </summary>
        /// <param name="devid"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ResumeAudioDevice(uint devid);

        /// <summary>
        /// bool SDLCALL BindAudioStream(AudioDeviceID devid, AudioStream *stream);
        /// </summary>
        /// <param name="devid"></param>
        /// <param name="stream"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool BindAudioStream(uint devid, IntPtr stream);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="devid"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static float GetAudioDeviceGain(uint devid);


        /// <summary>
        /// bool SDLCALL SetAudioDeviceGain(AudioDeviceID devid, float gain);
        /// </summary>
        /// <param name="devid"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetAudioDeviceGain(uint devid, float gain);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void CloseAudioDevice(uint devid);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void DestroyAudioStream(IntPtr stream);

        /// <summary>
        /// int put_audio_stream_data(AudioStream* stream, Uint8* data, Uint32 data_len)
        /// </summary>
        /// <param name="stream"></param>
        /// <param name="data"></param>
        /// <param name="len"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int PutAudioStreamData(IntPtr stream, byte[] data, uint len);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static ulong GetTicks();

        /// <summary>
        /// static Texture* create_texture_from_surface(Renderer* renderer, const char* file)
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateTextureFromSurface(Renderer renderer, string file);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GetTextureSize(Texture texture, out int w, out int h);

        /// <summary>
        /// bool SDLCALL RenderTextureRotated(Renderer *renderer, Texture *texture,const FRect* srcrect, const FRect* dstrect, double angle, const FPoint* center, FlipMode flip);
        /// </summary>
        /// <param name="texture"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool RenderTextureRotated(Renderer renderer, Texture texture, FRect srcRect, FRect destRect, double angle, FPoint center, FlipMode flip);

        /// <summary>
        /// void SDLCALL DestroyTexture(Texture *texture);
        /// </summary>
        /// <param name="texture"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void DestroyTexture(Texture texture);

        /// <summary>
        /// void * SDLCALL LoadFile(const char *file, size_t *datasize);
        /// </summary>
        /// <param name="texture"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr LoadFile(string file, out long datasize);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool PollEvent(out Event ev);

        /// <summary>
        ///  DECLSPEC int SDLCALL GetNumGPUDrivers(void);
        /// </summary>
        /// <param name="ev"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static int GetNumGPUDrivers();

        /// <summary>
        /// const char * SDLCALL GetGPUDriver(int index);
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GetGPUDriver(int index);

        /// <summary>
        /// bool SDLCALL ClaimWindowForGPUDevice(GPUDevice* device, Window* window);
        /// </summary>
        /// <param name="index"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool ClaimWindowForGPUDevice(IntPtr device, Window window);


        /// <summary>
        /// static GPUShader* create_vertex_shader(GPUDevice* device, const char* filename, const char* entrypoint)
        /// </summary>
        /// <param name="device"></param>
        /// <param name="window"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateVertexShader(IntPtr device, string filename, string entrypoint);

        /// <summary>
        ///  GPUShader* create_fragment_shader(GPUDevice* device, const char* filename, const char* entrypoint)
        /// </summary>
        /// <param name="device"></param>
        /// <param name="filename"></param>
        /// <param name="entrypoint"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateFragmentShader(IntPtr device, string filename, string entrypoint);


        /// <summary>
        /// GPUGraphicsPipeline* create_graphics_pipeline(GPUDevice* device, Window* window, GPUShader* vertex, GPUShader* fragment)
        /// </summary>
        /// <param name="device"></param>
        /// <param name="filename"></param>
        /// <param name="entrypoint"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateGraphicsPipeline(IntPtr device, Window window, IntPtr vertex, IntPtr fragment);

        /// <summary>
        /// static GPUBuffer* create_and_upload_vertex_data(GPUDevice* device, vertex_p* vertices, int vertices_len)
        /// </summary>
        /// <param name="device"></param>
        /// <param name="window"></param>
        /// <param name="vertex"></param>
        /// <param name="fragment"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateAndUploadVertexData(IntPtr device, Window window, IntPtr vertex, IntPtr fragment);


        /// <summary>
        /// static GPUBuffer* create_and_upload_indices_data(GPUDevice* device, uint32_t* indices, int indices_len)
        /// </summary>
        /// <param name="device"></param>
        /// <param name="window"></param>
        /// <param name="vertex"></param>
        /// <param name="fragment"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateAndUploadIndicesData(IntPtr device, out UInt32 indices, int indicesLen);

        /// <summary>
        /// static GPUTexture* create_gpu_texture(GPUDevice* device, const char* filename)
        /// </summary>
        /// <param name="device"></param>
        /// <param name="filename"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateGPUTexture(IntPtr device, string filename);

        /// <summary>
        /// static GPUSampler* create_gpu_sampler(GPUDevice* device)
        /// </summary>
        /// <param name="device"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateGPUSampler(IntPtr device);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr GPUUpdate(IntPtr device);

        /// <summary>
        /// void SDLCALL ReleaseGPUSampler(GPUDevice* device,GPUSampler *sampler);
        /// </summary>
        /// <param name="device"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ReleaseGPUSampler(IntPtr device);

        /// <summary>
        /// void SDLCALL ReleaseGPUTexture(GPUDevice* device,GPUTexture *texture);
        /// </summary>
        /// <param name="device"></param>
        /// <param name="texture"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ReleaseGPUTexture(IntPtr device, Texture texture);

        /// <summary>
        /// void SDLCALL ReleaseGPUBuffer(GPUDevice* device,GPUBuffer *buffer);
        /// </summary>
        /// <param name="device"></param>
        /// <param name="buffer"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ReleaseGPUBuffer(IntPtr device, IntPtr buffer);

        /// <summary>
        ///void SDLCALL ReleaseGPUGraphicsPipeline(GPUDevice* device,GPUGraphicsPipeline *graphics_pipeline);
        /// </summary>
        /// <param name="device"></param>
        /// <param name="pipeline"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ReleaseGPUGraphicsPipeline(IntPtr device, IntPtr pipeline);

        /// <summary>
        ///void SDLCALL ReleaseGPUShader(GPUDevice* device,GPUShader *shader);
        /// </summary>
        /// <param name="device"></param>
        /// <param name="shader"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ReleaseGPUShader(IntPtr device, IntPtr shader);

        /// <summary>
        ///void SDLCALL ReleaseWindowFromGPUDevice(GPUDevice* device,Window *window);
        /// </summary>
        /// <param name="device"></param>
        /// <param name="window"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void ReleaseWindowFromGPUDevice(IntPtr device, Window window);

        /// <summary>
        /// void SDLCALL DestroyGPUDevice(GPUDevice *device);
        /// </summary>
        /// <param name="device"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void DestroyGPUDevice(IntPtr device);

        /// <summary>
        /// MouseButtonFlags SDLCALL GetMouseState(float *x, float *y);
        /// </summary>
        /// <param name="x"></param>
        /// <param name="y"></param>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static uint GetMouseState(out float x, out float y);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="populated"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr CreateEnvironment(bool populated);

        /// <summary>
        /// const char* SDLCALL GetEnvironmentVariable(Environment* env, const char* name);
        /// </summary>
        /// <param name="env"></param>
        /// <param name="name"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static string GetEnvironmentVariable(IntPtr env, string name);

        /// <summary>
        /// bool SDLCALL SetEnvironmentVariable(Environment *env, const char *name, const char *value, bool overwrite);
        /// </summary>
        /// <param name="env"></param>
        /// <param name="name"></param>
        /// <param name="value"></param>
        /// <param name="overwrite"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetEnvironmentVariable(IntPtr env, string name, string value, bool overwrite);

        /// <summary>
        /// DECLSPEC void SDLCALL DestroyEnvironment(Environment *env);
        /// </summary>
        /// <param name="env"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static void DestroyEnvironment(IntPtr env);

        [MethodImpl(MethodImplOptions.InternalCall)]
        //public extern static IntPtr CreateThread(Func<IntPtr, int> fn, string name, IntPtr data);
        public extern static IntPtr CreateThread(Func<int> fn, string name);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static EventType GetEventType(IntPtr ev);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Scancode GetEventScancode(IntPtr ev);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Person GetPersonName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Person GetPersonAge();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static Person GetPersonScore();

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool SetRenderLogicalPresentation(IntPtr renderer, int w, int h, RendererLogicalPresentation mode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static bool GetRenderLogicalPresentation(IntPtr renderer, out int w, out int h, out RendererLogicalPresentation mode);
    }
}
