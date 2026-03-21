#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
 * Active slot tracker
 */
char active_slot = 'A';

/*
 * Get inactive slot
 */
char get_inactive_slot()
{
    return (active_slot == 'A') ? 'B' : 'A';
}

/*
 * Install firmware into inactive slot
 */
int install_firmware()
{
    char command[256];
    char target_slot = get_inactive_slot();

    sprintf(command,
        "cp firmware.bin firmware_slots/slot_%c/",
        target_slot);

    int ret = system(command);

    if(ret == 0)
    {
        printf("Firmware installed to Slot %c\n", target_slot);
        return 1;
    }

    printf("Firmware install failed\n");
    return 0;
}

/*
 * Switch active slot
 */
void switch_slot()
{
    active_slot = get_inactive_slot();
    printf("Switched to Slot %c\n", active_slot);
}

/*
 * Rollback mechanism
 */
void rollback()
{
    active_slot = (active_slot == 'A') ? 'B' : 'A';
    printf("Rollback to Slot %c\n", active_slot);
}
