#if !defined(HANDMADE_H)

/*
    NOTE

    HANDMADE_INTERNAL
        0 - Build for public release
        1 - Build for developer only
    HANDMADE_SLOW
        0 - No slow code allowed!
        1 - Slow code welcome
*/

// TODO: Implement sine ourselves
#include <math.h>
#include <stdint.h>

#define Pi32 3.1415926539f
#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef int32 bool32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

#if HANDMADE_SLOW
#define Assert(Expression) \
    if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

// TODO: Should these always use 64-bit?
#define Kilobytes(Value) (Value*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

// TODO: swap, min, max ... macros?

// TODO: Services that the platform layer provides to the game

#if HANDMADE_INTERNAL
// IMPORTANT
// THese are NOT for doing anything in the shipping game - they are
// blocking and the write doesn't protect against lost data!

struct debug_read_file_result
{
    uint32 ContentsSize;
    void *Contents;
};

struct thread_context
{
	int Placeholder;
};

#define DEBUG_PLATFORM_READ_ENTIRE_FILE(name) debug_read_file_result name(thread_context *Thread, char *FileName)
typedef DEBUG_PLATFORM_READ_ENTIRE_FILE(debug_platform_read_entire_file);

#define DEBUG_PLATFORM_FREE_FILE_MEMORY(name) void name(thread_context *Thread, void *Memory)
typedef DEBUG_PLATFORM_FREE_FILE_MEMORY(debug_platform_free_file_memory);

#define DEBUG_PLATFORM_WRITE_ENTIRE_FILE(name) bool32 name(thread_context *Thread, char *FileName, uint32 MemorySize, void *Memory)
typedef DEBUG_PLATFORM_WRITE_ENTIRE_FILE(debug_platform_write_entire_file);
#endif

// TODO: In the future, rendering _specifically_ will become a three-tiered abstraction
struct game_offscreen_buffer
{
    void *Memory;
    int Width;
    int Height;
    int Pitch;
	int BytesPerPixel;
};

struct game_sound_output_buffer
{
    int SamplesPerSecond;
    int SampleCount;
    int16 *Samples;
};

struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct game_controller_input
{
    bool32 IsConnected;
    bool32 IsAnalogue;

    real32 StickAverageX;
    real32 StickAverageY;

    union
	{
        game_button_state Buttons[12];
        struct
		{
            game_button_state MoveUp;
            game_button_state MoveDown;
            game_button_state MoveLeft;
            game_button_state MoveRight;

            game_button_state ActionUp;
            game_button_state ActionDown;
            game_button_state ActionLeft;
            game_button_state ActionRight;

            game_button_state LeftShoulder;
            game_button_state RightShoulder;

            game_button_state Back;
            game_button_state Start; // Start should be the last button for testing
        };
    };
};

struct game_input
{
	game_button_state MouseButtons[5];
	int32 MouseX, MouseY, MouseZ;

    // TODO: Insert clock value here
    game_controller_input Controllers[5];
};

inline game_controller_input *GetController(game_input *Input, int ControllerIndex)
{
    Assert(ControllerIndex < ArrayCount(Input->Controllers));
    return &Input->Controllers[ControllerIndex];
}

struct game_memory
{
    bool32 IsInitialized;

    uint64 PermanentStorageSize;
    void *PermanentStorage; // NOTE(casey): REQUIRED to be cleared to zero at startup

    uint64 TransientStorageSize;
    void *TransientStorage; // NOTE(casey): REQUIRED to be cleared to zero at startup

    debug_platform_read_entire_file *DEBUGPlatformReadEntireFile;
    debug_platform_free_file_memory *DEBUGPlatformFreeFileMemory;
    debug_platform_write_entire_file *DEBUGPlatformWriteEntireFile;
};

#define GAME_UPDATE_AND_RENDER(name) void name( thread_context *Thread, game_memory *Memory, game_input *Input, game_offscreen_buffer *Buffer )
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#define GAME_GET_SOUND_SAMPLES(name) void name( thread_context *Thread, game_memory *Memory, game_sound_output_buffer *SoundBuffer )
typedef GAME_GET_SOUND_SAMPLES(game_get_sound_samples);

struct game_state
{
    int ToneHz;
    int XOffset;
    int YOffset;

    real32 tSine;

	int PlayerX;
	int PlayerY;
	real32 tJump;
};

#define HANDMADE_H
#endif
