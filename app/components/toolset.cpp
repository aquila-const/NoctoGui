#include "toolset.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstddef>
#include <iostream>
#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <string>
#include <bits/stdc++.h>
#include <iterator>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <libudev.h>
#include <termios.h>

#define HASH1  0x10
#define HASH2  0x02
#define HASHSZ 512
#define USB_MAX_DEPTH 7
#define SYSFS_DEV_ATTR_PATH "/sys/bus/usb/devices/%s/%s"

#define ACK 0x79
#define NACK 0x1F
#define CMD_ERASE         0x43
#define CMD_GETID         0x02
#define CMD_WRITE         0x2b

#define APP_START_ADDRESS 0x08002C00 /* In STM32F070RB this corresponds with the start
                                      /*  address of Page 10 */

#define SRAM_SIZE         16*1024     // STM32F070RB has 16KB of RAM
#define SRAM_BASE         0x00000000
#define SRAM_END          (SRAM_BASE + SRAM_SIZE)
#define FLASH_TOTAL_PAGES 127

#define ENABLE_BOOTLOADER_PROTECTION 0
// #define PAGES_TO_PROTECT (OB_WRP_PAGES0TO1 | OB_WRP_PAGES2TO3 | OB_WRP_PAGES4TO5 | OB_WRP_PAGES6TO7)

Device::Device(const char *i, const char *c)
{
    device_name = i;
    manufacturer = c;
}

void Device::display()
{
    std::cout << "Device name " << device_name << "Manufacturer " << manufacturer << std::endl;
}
namespace Toolset {
    // struct flash_utils
    // {
    //     void _start(void);
    //     void _checik_and_set_flash_protection(void);
    //     void cmdErase(uint8_t *pucData);
    //     void cmdGetID(uint8_t *pucData);
    //     void cmdWrite(uint8_t *pucData);
    //     int entry_(void);
    //     void MX_CRC_Init(void);
    //     void MX_GPIO_Deinit(void);
    //     void MX_GPIO_Init(void);
    //     void MX_USART2_UART_Init(void);
    //     void SysTick_Handler();
        
    // };
    
    struct dev_signals
    {
        std::vector<std::pair<std::string, int>> baud_rates = {
            {"B0", B0},
            {"B50", B50},
            {"B75", B75},
            {"B110", B110},
            {"B134", B134},
            {"B150", B150},
            {"B200", B200},
            {"B300", B300},
            {"B600", B600},
            {"B1200", B1200},
            {"B1800", B1800},
            {"B2400", B2400},
            {"B4800", B4800},
            {"B9600", B9600},
            {"B19200", B19200},
            {"B38400", B38400},
            {"BCustom", 0x0161} //ie
        };
    };
    
    void init_bootloader()
    {
        
    }

    static unsigned int hashnum(unsigned int num)
    {
        unsigned int mask1 = (unsigned int)HASH1 << 27, mask2 = (unsigned int)HASH2 << 27;

        for (; mask1 >= HASH1; mask1 >>= 1, mask2 >>= 1)
            if (num & mask1)
                num ^= mask2;
        return num & (HASHSZ-1);
    }

    struct genericstrtable {
        struct genericstrtable *next;
        const unsigned int num;
        const char *name;
    };

    bool changed = false;

    static struct udev *udev = NULL;
    static struct udev_hwdb *hwdb = NULL;
    static struct genericstrtable *hiddescriptors_hash[HASHSZ] = { NULL, };
    static struct genericstrtable *reports_hash[HASHSZ] = { NULL, };
    static struct genericstrtable *huts_hash[HASHSZ] = { NULL, };
    static struct genericstrtable *biass_hash[HASHSZ] = { NULL, };
    static struct genericstrtable *physdess_hash[HASHSZ] = { NULL, };
    static struct genericstrtable *hutus_hash[HASHSZ] = { NULL, };
    static struct genericstrtable *langids_hash[HASHSZ] = { NULL, };
    static struct genericstrtable *countrycodes_hash[HASHSZ] = { NULL, };

    struct libusb_context *ctx;
    struct usb_obj {
        libusb_device **devs;
    };
    struct device_data {
        char device_name[256];
        char prod_id[256];
        char class_id[256];
        char *chipset[256];
    };
    struct device_ {
        int id;
        struct device_data dev_data;
        struct usb_obj *usb_data;
        std::vector<char*>  usb_device_list;
        int next;
    };
    struct USBDeviceGrp {
        struct device_data dev_data;
    };
    static const char *names_genericstrtable(struct genericstrtable *t[HASHSZ],
                        unsigned int idx)
    {
        struct genericstrtable *h;

        for (h = t[hashnum(idx)]; h; h = h->next)
            if (h->num == idx)
                return h->name;
        return NULL;
    }
    const char *names_hid(uint8_t hidd)
    {
        return names_genericstrtable(hiddescriptors_hash, hidd);
    }

    const char *names_reporttag(uint8_t rt)
    {
        return names_genericstrtable(reports_hash, rt);
    }

    const char *names_huts(unsigned int data)
    {
        return names_genericstrtable(huts_hash, data);
    }

    const char *names_hutus(unsigned int data)
    {
        return names_genericstrtable(hutus_hash, data);
    }

    const char *names_langid(uint16_t langid)
    {
        return names_genericstrtable(langids_hash, langid);
    }

    const char *names_physdes(uint8_t ph)
    {
        return names_genericstrtable(physdess_hash, ph);
    }

    const char *names_bias(uint8_t b)
    {
        return names_genericstrtable(biass_hash, b);
    }

    const char *names_countrycode(unsigned int countrycode)
    {
        return names_genericstrtable(countrycodes_hash, countrycode);
    }
   
    static const char *hwdb_get(const char *modalias, const char *key)
    {
        struct udev_list_entry *entry;

        udev_list_entry_foreach(entry, udev_hwdb_get_properties_list_entry(hwdb, modalias, 0))
            if (strcmp(udev_list_entry_get_name(entry), key) == 0)
                return udev_list_entry_get_value(entry);

        return NULL;
    }
    //TODO::change to vector of ids and check
    template <typename T, typename U, typename V, typename W, typename X>
    const char *names_parser(T vendorid, U productid, V classid, W subclassid, X protocolid)
    {
        if(vendorid != '\0')
        {
            char modalias[64];
            sprintf(modalias, "usb:v%04X*", vendorid);
            return hwdb_get(modalias, "ID_VENDOR_FROM_DATABASE");
        } //names_vendor
        
        if(vendorid != '\0' && productid != '\0')
        {
            char modalias[64];
            sprintf(modalias, "usb:v%04Xp%04X*", vendorid, productid);
            return hwdb_get(modalias, "ID_MODEL_FROM_DATABASE");
        }//names_product
        if(classid != '\0')
        {
            char modalias[64];
            sprintf(modalias, "usb:v*p*d*dc%02X*", classid);
            return hwdb_get(modalias, "ID_USB_CLASS_FROM_DATABASE");
        }
        if(classid != '\0' && subclassid != '\0')
        {
            char modalias[64];
            sprintf(modalias, "usb:v*p*d*dc%02Xdsc%02X*", classid, subclassid);
            return hwdb_get(modalias, "ID_USB_SUBCLASS_FROM_DATABASE");
        }
        if(classid != '\0' && subclassid != '\0' && protocolid != '\0')
        {
            char modalias[64];

            sprintf(modalias, "usb:v*p*d*dc%02Xdsc%02Xdp%02X*", classid, subclassid, protocolid);
            return hwdb_get(modalias, "ID_USB_PROTOCOL_FROM_DATABASE");
        }
        return NULL;
    }
    
    int get_vendor_string(char *buf, size_t size, uint16_t vid)
    {
            const char *cp;

            if (size < 1)
                    return 0;
            *buf = 0;
            if (!(cp = names_parser(vid, '\0', '\0', '\0', '\0')))
                    return 0;
            return snprintf(buf, size, "%s", cp);
    }

    int get_product_string(char *buf, size_t size, uint16_t vid, uint16_t pid)
    {
            const char *cp;

            if (size < 1)
                    return 0;
            *buf = 0;
            if (!(cp = names_parser(vid, pid, '\0', '\0', '\0')))
                    return 0;
            return snprintf(buf, size, "%s", cp);
    }

    int get_class_string(char *buf, size_t size, uint8_t cls)
    {
        const char *cp;

        if (size < 1)
            return 0;
        *buf = 0;
        if (!(cp = names_parser('\0', '\0', cls, '\0', '\0')))
            return 0;
        return snprintf(buf, size, "%s", cp);
    }

    int get_subclass_string(char *buf, size_t size, uint8_t cls, uint8_t subcls)
    {
        const char *cp;

        if (size < 1)
            return 0;
        *buf = 0;
        if (!(cp = names_parser('\0', '\0', cls, subcls, '\0')))
            return 0;
        return snprintf(buf, size, "%s", cp);
    }
    static int get_protocol_string(char *buf, size_t size, uint8_t cls, uint8_t subcls, uint8_t proto)
    {
        const char *cp;

        if (size < 1)
            return 0;
        *buf = 0;
        if (!(cp = names_parser('\0', '\0', cls, subcls, proto)))
            return 0;
        return snprintf(buf, size, "%s", cp);
    }

    int get_sysfs_name(char *buf, size_t size, libusb_device *dev)
    {
        int len = 0;
        uint8_t bnum = libusb_get_bus_number(dev);
        uint8_t pnums[USB_MAX_DEPTH];
        int num_pnums;

        buf[0] = '\0';

        num_pnums = libusb_get_port_numbers(dev, pnums, sizeof(pnums));
        if (num_pnums == LIBUSB_ERROR_OVERFLOW) {
            return -1;
        } else if (num_pnums == 0) {
            /* Special-case root devices */
            return snprintf(buf, size, "usb%d", bnum);
        }

        len += snprintf(buf, size, "%d-", bnum);
        for (int i = 0; i < num_pnums; i++)
            len += snprintf(buf + len, size - len, i ? ".%d" : "%d", pnums[i]);

        return len;
    }
    int read_sysfs_prop(char *buf, size_t size, char *sysfs_name, char *propname)
    {
        int n, fd;
        char path[PATH_MAX];

        buf[0] = '\0';
        snprintf(path, sizeof(path), SYSFS_DEV_ATTR_PATH, sysfs_name, propname);
        fd = open(path, O_RDONLY);

        if (fd == -1)
            return 0;

        n = read(fd, buf, size);

        if (n > 0)
            buf[n-1] = '\0';  // Turn newline into null terminator

        close(fd);
        return n;
    }

    void
    update_device_state(libusb_device **devs, struct device_data dev_data)
    {
        //char *acquired_device_name = Toolset::acquire_device(devs, dev_data);
        struct device_ dev_;
        // dev_data.device_name_list = acquire_device(devs, dev_data, dev_);
        std::cout << "Device name -> " << dev_data.device_name << std::endl;
    }

    bool dirty = false;

    bool header(const char *caption)
    {
        return ImGui::CollapsingHeader(caption, ImGuiTreeNodeFlags_DefaultOpen);
    }

    bool ImGuiComboUI(const std::string &caption, int item_current_idx, ImVector<Device> items)
    {
        const char* combo_preview_value = items[item_current_idx].device_name;  // Pass in the preview value visible before opening the combo (it could be anything)

        if (ImGui::BeginCombo(caption.c_str(), combo_preview_value))
        {
            for (int n = 0; n < items.size(); n++)
            {
                const bool is_selected = (item_current_idx == n);
                ImGui::PushID(n);
                if (ImGui::Selectable(items[n].device_name, is_selected))
                {
                    item_current_idx = n;
                    changed = true;
                }
                ImGui::PopID();
                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            
            ImGui::EndCombo();
        }

        return changed;
    }

    ImVector<Device> populate_list()
    {
        libusb_context *context = NULL;
        libusb_device **list = NULL;
        int rc = 0;
        ssize_t count = 0;
        char string_[256];
        rc = libusb_init(&context);
        assert(rc == 0);
        bool imgui_ctx = false;
        count = libusb_get_device_list(context, &list);
        assert(count > 0);
        std::string output_str = "'\0'";
        
        ImVector<Device> items_list = {};
        std::vector<Device> devices;
        items_list.reserve(count);//reserve to reduce overflows
        Device *devices_l;
  
        std::string search_string = "";
        
        
        
        for (size_t idx = 0; idx < count; ++idx)
        {
            
            struct device_ device_list;
            libusb_device *device = list[idx];
            char vendor[128], product[128];
            char cls[128];
            char subcls[128];
            char proto[128];
            char mfg[128] = {0};
            char prod[128] = {0};
            char serial[128] = {0};
            char sysfs_name[PATH_MAX];
            libusb_device_descriptor desc = {0};
            libusb_device_handle *handle = {0};
            rc = libusb_get_device_descriptor(device, &desc);
            assert(rc == 0);
            get_vendor_string(vendor, sizeof(vendor), desc.idVendor);
            get_product_string(product, sizeof(product),
                    desc.idVendor, desc.idProduct);
            get_class_string(cls, sizeof(cls), desc.bDeviceClass);
            get_subclass_string(subcls, sizeof(subcls),
                    desc.bDeviceClass, desc.bDeviceSubClass);
            get_protocol_string(proto, sizeof(proto), desc.bDeviceClass,    
                    desc.bDeviceSubClass, desc.bDeviceProtocol);

            if (get_sysfs_name(sysfs_name, sizeof(sysfs_name), device) >= 0)
            {
                read_sysfs_prop(mfg, sizeof(mfg), sysfs_name, "manufacturer");
                read_sysfs_prop(prod, sizeof(prod), sysfs_name, "product");
                read_sysfs_prop(serial, sizeof(serial), sysfs_name, "serial");
            }
        
            items_list.push_back(Device(
                mfg,
                prod
            ));
        }

        libusb_free_device_list(list, count);
        libusb_exit(context);

        return items_list;
    }
    
    bool DeviceWidget(struct device_ dev)
    {
        
        std::multimap<std::string, char*> s_map;
        std::string current_key = "";
        std::string caption = "##combo";
        
        ImVector<Device> device_list = populate_list();
        
        bool imgui_ctx = false;
        
        static int item_current_idx = 0; // Here we store our selection data as an index.
        
        imgui_ctx = ImGuiComboUI(caption, item_current_idx, device_list);
        
        return imgui_ctx;
    }
    bool DeviceBaudRateWidget(struct device_ dev_)
    {
        struct dev_signals d_s;
        std::vector<std::pair<std::string, int>> baud_rates = d_s.baud_rates;
        std::vector<const char *> items_list = {};
        items_list.reserve(sizeof(baud_rates));
        for(auto iter : baud_rates)
        {
            // std::cout << iter.first << " " << iter.second << std::endl;
            items_list.emplace_back(iter.first.c_str());
        }
        std::string caption = "##baudrates";
        static int item_current_idx = 0;
        // ImGuiComboUI(caption, item_current_idx, items_list);
        return true;
    }
    bool DeviceStatsWidget(struct device_ dev_)
    {
        bool is_ok = true;
        ImGui::Text("Chipset %c", dev_.dev_data.chipset);    
        is_ok = DeviceBaudRateWidget(dev_);    
        return is_ok;
    }

    void FlasherView()
    {
        struct device_ dev_;

        ImGui::Begin("Device");
        ImGui::Spacing();

        ImGui::Text("Name");

        if(changed)
        {
            ImGui::SameLine(); ImGui::Text("N/A");
        } else {
            ImGui::SameLine(); ImGui::Text("None");
        }
        
        DeviceStatsWidget(dev_);
        DeviceWidget(dev_);

        if(ImGui::Button("Flash Device"))
        {
            std::cout << "Testing..." << std::endl;
        }
        static ExampleAppConsole console;
        console.Draw("Console", nullptr);
        ImGui::End();
    }
}
