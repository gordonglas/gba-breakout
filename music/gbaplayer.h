/*************************************************************
/ GBA Module Player    Copyright (c) 2001 Sergej Kravcenko   /
/ Version 0.9b         sergej@codewaves.com                  /
*************************************************************/

#ifndef GBAPLAYER
#define GBAPLAYER

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////// Structures ////////////////////////////
typedef struct
{
   const unsigned char  *lpbSample;    // Sample data in 8-bit mono RAW  format
   unsigned char        bLeft;         // 0 - left channel, non-0 - right channel
   unsigned long        lStart;        // Start position
   unsigned long        lSize;         // Sample size
   unsigned long        lRepStart;     // Loop start
   unsigned long        lRepEnd;       // Loop end
   unsigned char        bVolume;       // Sound effect volume
   unsigned char        bRep;          // 0 - disable looping, non-0 - enable looping
   unsigned long        lFrq;          // Sample frequency (max 21600 Hz)
} SFXINFO;

/////////////////////////// Prototypes ///////////////////////////////

// Function:      MP_IntHandler
// Description:   DMA1 interrupt handler
// Parameters:    None
// 
// Comments:      You must enable interupts and add this handle to interrupt table (DMA1 slot). 
//void MP_IntHandler(void);
__attribute__(( long_call )) void MP_IntHandler( void );


// Function:      MP_InitArm
// Description:   Inits gba music player and sound engine
// Parameters:    None
// 
// Comments:      Make sure that dma interrupt handler is set before using this function
void MP_InitArm(void);
void MP_InitThumb(void);

// Function:      MP_UpdateArm
// Description:   Updates current player status
// Parameters:    None
// 
// Comments:      You must call this function every GBA screen frame. It doesn't 
//                matter when you call this function - at frame start or frame end.
// WARNING!!!     This call will enable interrupts and disable all interrupts
//                except DMA1 interrupt. Call this function first and then enable all
//                your interrupts.
//void MP_UpdateArm(void);
__attribute__(( long_call )) void MP_UpdateArm(void);
void MP_UpdateThumb(void);

// Function:      MP_PlayModuleArm
// Description:   Sets current module for playback.
// Parameters:    lpbModule -> pointer to module data
//                bLoop     -> loop module
// 
// Comments:      You can call this function when you want to play new module or change
//                current module
void MP_PlayModuleArm(const unsigned char *lpbModule, unsigned char bLoop);
void MP_PlayModuleThumb(const unsigned char *lpbModule, unsigned char bLoop);

// Function:      MP_StopModuleArm
// Description:   Stops module playback
// Parameters:    None
void MP_StopModuleArm(void);
void MP_StopModuleThumb(void);

// Function:      MP_ResumeModuleArm
// Description:   Resumes module playback
// Parameters:    None
void MP_ResumeModuleArm(void);
void MP_ResumeModuleThumb(void);

// Function:      MP_SetVolumeArm
// Description:   Sets module volume
// Parameters:    bVol - module volume (0-100)
void MP_SetVolumeArm(unsigned char bVol);
void MP_SetVolumeThumb(unsigned char bVol);

// Function:      MP_PlaySfxArm
// Description:   Plays sound effect
// Parameters:    lpsfxInfo -> sound effect information (see struct description for more info)
void MP_PlaySfxArm(SFXINFO *lpsfxInfo);
void MP_PlaySfxThumb(SFXINFO *lpsfxInfo);

// Function:      MP_StopSfxArm
// Description:   Stops sound effect playback
// Parameters:    bLeft -> 0 - left channel, non-0 - right channel
// 
// Comments:      This function is usefull when you want to stop sound effect with
//                looping
void MP_StopSfxArm(unsigned char bLeft);
void MP_StopSfxThumb(unsigned char bLeft);

#ifdef __cplusplus
}
#endif

#endif
