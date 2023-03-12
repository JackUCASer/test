#ifndef __MENUFUNSET_H
#define __MENUFUNSET_H

//#include "init.h"
#include "datatype.h"

void DispMenuVersionsInit(void);
void DispMenuVersionsON(void);
void DispMenuVersionsSel(void);
void DispMenuVersionsUp(void);
void DispMenuVersionsDown(void);

void DispMenuAutoPowerOffInit(void);
void DispMenuAutoPowerOffON(void);
void DispMenuAutoPowerOffSel(void);
void DispMenuAutoPowerOffUp(void);
void DispMenuAutoPowerOffDown(void);

void DispMenuAutoStandyInit(void);
void DispMenuAutoStandyON(void);
void DispMenuAutoStandySel(void);
void DispMenuAutoStandyUp(void);
void DispMenuAutoStandyDown(void);

void DispMenuBeepVolInit(void);
void DispMenuBeepVolON(void);
void DispMenuBeepVolSel(void);
void DispMenuBeepVolUp(void);
void DispMenuBeepVolDown(void);

void DispMenuBlInit(void);
void DispMenuBlON(void);
void DispMenuBlSel(void);
void DispMenuBlUp(void);
void DispMenuBlDown(void);

void DispMenuHandInit(void);
void DispMenuHandON(void);
void DispMenuHandSel(void);
void DispMenuHandUp(void);
void DispMenuHandDown(void);

void DispMenuStartupMemoryInit(void);
void DispMenuStartupMemoryON(void);
void DispMenuStartupMemorySel(void);
void DispMenuStartupMemoryUp(void);
void DispMenuStartupMemoryDown(void);

void DispMenuApexsensitivityInit(void);
void DispMenuApexsensitivityON(void);
void DispMenuApexsensitivitySel(void);
void DispMenuApexsensitivityUp(void);
void DispMenuApexsensitivityDown(void);

void DispMenuLanguageInit(void);
void DispMenuLanguageON(void);
void DispMenuLanguageSel(void);
void DispMenuLanguageUp(void);
void DispMenuLanguageDown(void);

void DispMenuCalibrationInit(void);
void DispMenuCalibrationON(void);
void DispMenuCalibrationSel(void);
void DispMenuCalibrationUp(void);
void DispMenuCalibrationDown(void);

void DispMenuRestoreFactorySettingsInit(void);
void DispMenuRestoreFactorySettingsON(void);
void DispMenuRestoreFactorySettingsSel(void);
void DispMenuRestoreFactorySettingsUp(void);
void DispMenuRestoreFactorySettingsDown(void);


void DispMenuUndefineInit(void);
void DispMenuUndefineON(void);
void DispMenuUndefineSel(void);
void DispMenuUndefineUp(void);
void DispMenuUndefineDown(void);


void DispMenuBuiltInFileInit(void);
void DispMenuBuiltInFileON(void);
void DispMenuBuiltInFileSel(void);
void DispMenuBuiltInFileUp(void);
void DispMenuBuiltInFileDown(void);
void DispMenuBuiltInFileLongset(void);

uint8_t IsParameterChange(int FileLibraryNum);

void DispMenuRenameFileInit(void);
void DispMenuRenameFileON(void);
void DispMenuRenameFileSel(void);
void DispMenuRenameFileUp(void);
void DispMenuRenameFileDown(void);
void DispMenuRenameFileLongSel(void);

void DispMenuFilelistInit(void);
void DispMenuFilelistON(void);
void DispMenuFilelistSel(void);
void DispMenuFilelistUp(void);
void DispMenuFilelistDown(void);
void DispMenuFilelistLongSel(void);

void DispFileDeleteInit(void);
void DispFileDeleteON(void);
void DispFileDeleteSel(void);
void DispFileDeleteDown(void);
void DispFileDeleteUp(void);

#endif