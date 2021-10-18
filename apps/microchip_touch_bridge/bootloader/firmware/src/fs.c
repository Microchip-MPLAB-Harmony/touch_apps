/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    fs.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

#include "fs.h"
#include "hex_parser.h"
#include "common.h"

/*************************************************
** these contents are created using python file **
*************************************************/
uint8_t readme[206] = {
	0x3C, 0x68, 0x74, 0x6D, 0x6C, 0x3E, 0x0A, 0x3C, 0x68, 0x65, 0x61, 0x64, 0x3E, 0x0A, 0x3C, 0x6D, 
	0x65, 0x74, 0x61, 0x20, 0x68, 0x74, 0x74, 0x70, 0x2D, 0x65, 0x71, 0x75, 0x69, 0x76, 0x3D, 0x22, 
	0x72, 0x65, 0x66, 0x72, 0x65, 0x73, 0x68, 0x22, 0x20, 0x63, 0x6F, 0x6E, 0x74, 0x65, 0x6E, 0x74, 
	0x3D, 0x22, 0x30, 0x3B, 0x20, 0x75, 0x72, 0x6C, 0x3D, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 
	0x2F, 0x6D, 0x69, 0x63, 0x72, 0x6F, 0x63, 0x68, 0x69, 0x70, 0x64, 0x65, 0x76, 0x65, 0x6C, 0x6F, 
	0x70, 0x65, 0x72, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x74, 0x6F, 0x75, 0x63, 0x68, 0x3A, 0x69, 0x6E, 
	0x74, 0x72, 0x6F, 0x64, 0x75, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x2D, 0x74, 0x6F, 0x2D, 0x6D, 0x69, 
	0x63, 0x72, 0x6F, 0x63, 0x68, 0x69, 0x70, 0x2D, 0x74, 0x6F, 0x75, 0x63, 0x68, 0x2D, 0x62, 0x72, 
	0x69, 0x64, 0x67, 0x65, 0x22, 0x2F, 0x3E, 0x0A, 0x3C, 0x74, 0x69, 0x74, 0x6C, 0x65, 0x3E, 0x52, 
	0x65, 0x64, 0x69, 0x72, 0x65, 0x63, 0x74, 0x20, 0x74, 0x6F, 0x20, 0x4D, 0x69, 0x63, 0x72, 0x6F, 
	0x63, 0x68, 0x69, 0x70, 0x2E, 0x63, 0x6F, 0x6D, 0x3C, 0x2F, 0x74, 0x69, 0x74, 0x6C, 0x65, 0x3E, 
	0x0A, 0x3C, 0x2F, 0x68, 0x65, 0x61, 0x64, 0x3E, 0x0A, 0x3C, 0x62, 0x6F, 0x64, 0x79, 0x3E, 0x3C, 
	0x2F, 0x62, 0x6F, 0x64, 0x79, 0x3E, 0x0A, 0x3C, 0x2F, 0x68, 0x74, 0x6D, 0x6C, 0x3E, 
};

uint8_t kitinfo[159] = {
	0x4D, 0x69, 0x63, 0x72, 0x6F, 0x63, 0x68, 0x69, 0x70, 0x20, 0x54, 0x6F, 0x75, 0x63, 0x68, 0x20, 
	0x42, 0x72, 0x69, 0x64, 0x67, 0x65, 0x0A, 0x0A, 0x46, 0x69, 0x72, 0x6D, 0x77, 0x61, 0x72, 0x65, 
	0x20, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 0x6E, 0x3A, 0x0A, 0x42, 0x6F, 0x6F, 0x74, 0x20, 0x4C, 
	0x6F, 0x61, 0x64, 0x65, 0x72, 0x3A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x0A, 0x0A, 0x53, 0x74, 0x61, 0x74, 0x75, 0x73, 0x3A, 0x0A, 0x56, 0x61, 0x6C, 0x69, 0x64, 
	0x20, 0x41, 0x70, 0x70, 0x6C, 0x69, 0x63, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x3A, 0x20, 0x59, 0x65, 
	0x73, 0x0A, 0x53, 0x77, 0x69, 0x74, 0x63, 0x68, 0x20, 0x50, 0x6F, 0x73, 0x3A, 0x20, 0x30, 0x30, 
	0x30, 0x30, 0x0A, 0x0A, 0x52, 0x65, 0x66, 0x65, 0x72, 0x20, 0x74, 0x6F, 0x20, 0x52, 0x45, 0x41, 
	0x44, 0x4D, 0x45, 0x2E, 0x48, 0x54, 0x4D, 0x20, 0x66, 0x6F, 0x72, 0x20, 0x6D, 0x6F, 0x72, 0x65, 
	0x20, 0x69, 0x6E, 0x66, 0x6F, 0x72, 0x6D, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x21, 0x21, 0x21, 
};

uint8_t fat[6] = {
	0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
};

uint8_t mbr[62] = {
	0xEB, 0x3C, 0x90, 0x4d, 0x53, 0x44, 0x4f, 0x53, 0x35, 0x2e, 0x30, 0x00, 0x02, 0x08, 0x01, 0x00, 
	0x02, 0x10, 0x00, 0x00, 0x00, 0xF8, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x80, 0x08, 0x00, 0x00, 0x80, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x54, 0x43, 0x48, 0x20, 0x42, 
	0x52, 0x49, 0x44, 0x47, 0x45, 0x20, 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 
};

uint8_t rootdir[96] = {
	0x54, 0x43, 0x48, 0x20, 0x42, 0x52, 0x49, 0x44, 0x47, 0x45, 0x20, 0x28, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x11, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x52, 0x45, 0x41, 0x44, 0x4d, 0x45, 0x20, 0x20, 0x48, 0x54, 0x4d, 0x01, 0x00, 0x06, 0x28, 0x78, 
	0xDE, 0x38, 0xDE, 0x38, 0x00, 0x00, 0x04, 0x77, 0xDE, 0x38, 0x02, 0x00, 0xCE, 0x00, 0x00, 0x00, 
	0x4b, 0x49, 0x54, 0x49, 0x4e, 0x46, 0x4f, 0x20, 0x54, 0x58, 0x54, 0x01, 0x00, 0x06, 0x28, 0x78, 
	0xDE, 0x38, 0xDE, 0x38, 0x00, 0x00, 0x04, 0x77, 0xDE, 0x38, 0x03, 0x00, 0x9F, 0x00, 0x00, 0x00, 
};

typedef struct tag {
    uint8_t *srcptr;
    uint16_t length;
}filesystem_t;

filesystem_t filesystem[] = {
    { &mbr[0], sizeof(mbr) },
    { &fat[0], sizeof(fat) },
    { &fat[0], sizeof(fat) },
    { &rootdir[0], sizeof(rootdir) },
    { &readme[0], sizeof(readme) },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { &kitinfo[0], sizeof(kitinfo) },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
    { 0, 0 },
};

/*************************************************/

const unsigned char KEEP(BT_VERSION_ADDR) btver[VERSION_LENGTH] = BOOTLOADER_VERSION;

void fsUpdateInfoFile(uint8_t *data) {

	uint8_t app_status[APP_STATUS_LENGTH] = "No "; // by default "Yes" is there
	uint8_t switch_status[SWITCH_STATUS_LENGTH] = "0000"; // default "0000"
	uint8_t status_info[64];
	uint8_t *ptr;

	for(uint8_t cnt = 0; cnt < SWITCH_STATUS_LENGTH; cnt++) {
		if(appConfig.switch_positions & (1<<cnt)) {
			switch_status[SWITCH_STATUS_LENGTH-1-cnt] = '1';
		}
	}

//	memcpy(status_info, (uint8_t *) BT_VERSION_ADDR, VERSION_LENGTH);
	ptr = (uint8_t *) &data[BT_VERSION_POS];
	memcpy(ptr, commonStatus.bl_version, VERSION_LENGTH);

	memcpy(status_info, (uint8_t *) APP_START_ADDR, 4);
	if(status_info[0] == 0xFF && status_info[1] == 0xFF && status_info[2] == 0xFF && status_info[3] == 0xFF) {
		ptr = (uint8_t *) &data[APP_STATUS_POS];
		memcpy(ptr, app_status, APP_STATUS_LENGTH);
	}

	ptr = (uint8_t *) &data[SWITCH_STATUS_POS];
	memcpy(ptr, switch_status, SWITCH_STATUS_LENGTH);
}

/**
 * @brief Returns ture if the address is point to root sector. ROOT is available in sector 3.
 * 
 * @param address 
 * @return true if the address is pointing to ROOT address
 * @return false if the address is NOT pointing to ROOT address
 */
bool virtual_fat12_is_addressing_fat (uint32_t address)
{
    /* ROOT is available in sector 3. 3*SECTOR_SIZE <= (4*SECTOR_SIZE)-1 */
    if((address >= (APP_START_ADDR+0x600)) && address <= ((APP_START_ADDR+0x7FF))) {
        return true;
    }
    return false;
}

/**
 * @brief Returns sector number based on the address passed. 
 * 
 * @param address address sent by USB host
 * @return secotr number for hte given address
 */
uint16_t fsgetSectorNumber(uint32_t address){
    /* Address passed is offset by APP_START_ADDR. */
    uint32_t temp_address = address - APP_START_ADDR;
    /* div by 512 is euqual to >> 9 */
    temp_address = temp_address >> 9;
    
    return (uint16_t) temp_address;
}

/**
 * @brief Returns the offset address when address is not multiple of 512
 * 
 * @param address 
 * @return uint32_t 
 */
uint32_t fsgetOffSetAddr(uint32_t address){
    uint32_t temp_address = address - APP_START_ADDR;
    temp_address = temp_address & (SECTOR_SIZE-1);
    
    return (uint32_t) temp_address;
}

MEMORY_DEVICE_TRANSFER_STATUS fsTransferStatusGet( const DRV_HANDLE handle )
{
    return DRV_NVMCTRL_TransferStatusGet(handle);
}

bool fsRead( const DRV_HANDLE handle, void * rx_data, uint32_t rx_data_length, uint32_t address )
{
    /* USB is expecting address 0x20000 */
    uint16_t sector_number = fsgetSectorNumber(address);
    
    uint32_t temp_size = elements(filesystem);

    memset((uint8_t*)rx_data, 0, rx_data_length);

    /* if the sector which host is trying is access is more than our virtual content, sent just zero */
    if(sector_number < temp_size)
    {
        filesystem_t *pfs = &filesystem[sector_number];
		if(pfs->srcptr != 0)
        	memcpy (rx_data, pfs->srcptr, pfs->length);
    }

    /* for master boot record, update these position - otherwise it doss not work */
	if(sector_number == 0){
		uint8_t *ptr = (uint8_t *) rx_data;
		ptr[62] = 0xCD;
		ptr[63] = 0x18;
		ptr[510] = 0x55;
		ptr[511] = 0xAA;
	} else if(sector_number == 12){
        /* KIT info file - updated with proper status before sending to host */
		fsUpdateInfoFile((uint8_t *) rx_data);
	}

    return true;
}


bool fsPageWrite( const DRV_HANDLE handle, void *tx_data, uint32_t address )
{
    uint8_t *paddress;
    /* USB is expecting address 0x20000 */
    uint16_t sector_number = fsgetSectorNumber(address);
    paddress = (uint8_t *) filesystem[sector_number].srcptr;
    uint32_t offset = fsgetOffSetAddr(address);
    uint32_t temp_size = elements(filesystem);

    fsWriteProcess(tx_data, address, sector_number);

    if(sector_number < temp_size)
    {
        paddress = (uint8_t *) filesystem[sector_number].srcptr;
        paddress += offset;
        uint32_t size = filesystem[sector_number].length;
        if(offset <= size){
            // memcpy( (uint8_t*) paddress, (uint8_t*) tx_data, size);
        } else {
            // memcpy( (uint8_t*) paddress, (uint8_t*) tx_data, 64);
        }
    }
    return true;
}

bool fsSectorErase( const DRV_HANDLE handle, uint32_t address )
{
    return true;
}


bool fsGeometryGet( const DRV_HANDLE handle, MEMORY_DEVICE_GEOMETRY *geometry )
{
    /* Read block size and number of blocks */
    geometry->read_blockSize = 1;
    geometry->read_numBlocks = (DRV_MEMORY_DEVICE_MEDIA_SIZE * 1024);

    /* Write block size and number of blocks */
    geometry->write_blockSize = DRV_MEMORY_DEVICE_PROGRAM_SIZE;
    geometry->write_numBlocks = ((DRV_MEMORY_DEVICE_MEDIA_SIZE * 1024) / DRV_MEMORY_DEVICE_PROGRAM_SIZE);

    /* Erase block size and number of blocks */
    geometry->erase_blockSize = DRV_MEMORY_DEVICE_ERASE_SIZE;
    geometry->erase_numBlocks = ((DRV_MEMORY_DEVICE_MEDIA_SIZE * 1024) / DRV_MEMORY_DEVICE_ERASE_SIZE);

    geometry->numReadRegions = 1;
    geometry->numWriteRegions = 1;
    geometry->numEraseRegions = 1;

    geometry->blockStartAddress = 0x10000;

    return true;
}

DRV_HANDLE fsOpen( const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent )
{
    return DRV_NVMCTRL_Open(drvIndex, ioIntent);
}

void fsClose( const DRV_HANDLE handle )
{
    DRV_NVMCTRL_Close(handle);
}

SYS_STATUS fsStatus( const SYS_MODULE_INDEX drvIndex )
{
    return DRV_NVMCTRL_Status(drvIndex);
    /* Return the driver status */
}



// Is this a valid hexfile?
bool validate_hexfile(uint8_t *buf)
{
    // look here for known hex records
    return ((buf[0] == ':') && ((buf[8] == '0') || (buf[8] == '2') || (buf[8] == '3') || (buf[8] == '4') || (buf[8] == '5')));
}

// Checks if a filename is valid as FAT entry
static bool filename_valid(uint8_t c)
{
    const char *valid_char = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    do {
        if (c == *valid_char) {
            return true;
        }
    } while ((uint8_t)(*(valid_char++)) != '\0');
    return false;
}

static const char *const known_extensions[] = {
    "HEX",
    "hex",
    0,
};

// Checks if a file entry is acceptable in the FAT12
static bool acceptable_fat_entry(const FatDirectoryEntry_t dir_entry)
{
    uint32_t i = 0;
    // we may see the following. Validate all or keep looking
    //  entry with invalid or reserved first byte
    //  entry with a false filesize.
    while (known_extensions[i] != 0) {
        if(1 == filename_valid(dir_entry.filename[0])) {
            if (0 == strncmp(known_extensions[i], (const char *)&dir_entry.filename[8], 3)) {
                return (dir_entry.filesize);
            }
        }
        i++;
    }
    return false;
}

// Examine the start contents
bool parse_start_of_file(uint8_t *buf)
{
    // First try parse it as a hex file
    if (validate_hexfile(buf)) {
        return true;
    } else {
        return false;
    }
}

typedef enum {
    YET_TO_FIND_HEX_FILE = 0,
    HEX_FILE_FOUND,
    HEX_FILE_RECEIVED
}fileProcess_t;

fileProcess_t fileProcess;
uint32_t hexFileSizeExpected, hexFileSizeReceived;
uint8_t fileProcessNoError = 1;
extern uint32_t swtichToAppDelay;

void fsWriteProcess(uint8_t *data, uint32_t addrIn, uint16_t secnum) {
    static uint8_t pmsd_data_frame[512];
    static uint16_t cnt = 0;
    // static uint32_t addr;
    // if(cnt == 0) {
    //     addr = addrIn;
    // }
    uint8_t *ptr = &pmsd_data_frame[cnt];
    cnt += 64;
    memcpy(ptr, data, 64);
    
    if(cnt < 512){
        return;
    }
    cnt = 0;

    // If the root dir comes we should look at it and parse for info that can end a transfer
    if (virtual_fat12_is_addressing_fat (addrIn))
    {
        // Look in the FAT entry to extract the filesize, since its sent upfront
        uint32_t i = 0;

        // File system entry
        FatDirectoryEntry_t tmp_file = {0};

        SYS_CONSOLE_Print(sysObj.sysConsole0, "USB Host Access FAT\n");

        while (i < (SECTOR_SIZE/sizeof(tmp_file))) {
            memcpy(&tmp_file, (pmsd_data_frame + (i*sizeof(tmp_file))), sizeof(tmp_file));
            if (acceptable_fat_entry(tmp_file)) {
                if(fileProcess == YET_TO_FIND_HEX_FILE) {
                    SYS_CONSOLE_Print(sysObj.sysConsole0, "HEX Copied\n");
                    SYS_CONSOLE_Print(sysObj.sysConsole0, "Size Expected= ");
                    print32(tmp_file.filesize);
                    SYS_CONSOLE_Print(sysObj.sysConsole0, "\n");
                    hexFileSizeExpected = tmp_file.filesize;
                }

                if((hexFileSizeReceived >= hexFileSizeExpected) && (fileProcess == HEX_FILE_RECEIVED)) {
                    if(fileProcessNoError == 1) {
                        SYS_CONSOLE_Print(sysObj.sysConsole0, "Switching to application...\n");
                        LED_GREEN_Clear();
                        swtichToAppDelay = 500000;
                    } else {
                        SYS_CONSOLE_Print(sysObj.sysConsole0, "Error Restarting...\n");
                        LED_RED_Clear();
                        swtichToAppDelay = 500000;
                    }
                }
                break;
            }
            i++;
        }
    }

    switch(fileProcess) {
        case YET_TO_FIND_HEX_FILE:
            if(parse_start_of_file((uint8_t *) pmsd_data_frame)) {
                fileProcess = HEX_FILE_FOUND;
                SYS_CONSOLE_Print(sysObj.sysConsole0, "Start Hex Parser\n");
                hex_parser_init();
                hexFileSizeReceived += 512;
                hex_parser_process(pmsd_data_frame, 512);
                //SYS_CONSOLE_Print(sysObj.sysConsole0, "Received %08x\n", hexFileSizeReceived);
            } else {
                // break;
            }
        break;
        case HEX_FILE_FOUND:
            hexFileSizeReceived += 512;
            hex_parser_process(pmsd_data_frame, 512);
            // SYS_CONSOLE_Print(sysObj.sysConsole0, "Received 0x%08x\n", hexFileSizeReceived);
			if(secnum == 3) {
                SYS_CONSOLE_Print(sysObj.sysConsole0, "Received 0x%08x\n", hexFileSizeReceived);
                SYS_CONSOLE_Print(sysObj.sysConsole0, "HEX Close\n");
				fileProcess = HEX_FILE_RECEIVED;
			}
            break;
        default:
        break;
    }
}

