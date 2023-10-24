

// for std::min
#define NOMINMAX

// needs to be included before browser
#include "_windows_ping.hpp"
#include "_windows_domain.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS // https://github.com/ocornut/imgui/issues/2832
#include "DashboardManager.h"
// DashboardManager dashboardManager;

std::condition_variable cv; // for stopping pinging early
std::mutex cv_m; // for stopping pinging early

//extern ImFont* font_industry_bold;
//extern ImFont* font_industry_medium;

extern ImFont* font_title;
extern ImFont* font_subtitle;
extern ImFont* font_text;

extern OPTIONS options;
extern AppStore appStore;

extern FirewallManager firewallManager;

// this value is combined with the new ping.
static const unsigned char ping_offset = 10;

void DashboardManager::startPinging(int interval = 9000)
{

    std::thread thread = std::thread([&, interval]()
    {
        while (this->pinging)
        {
            for (auto& endpoint : this->endpoints)
            {
                std::thread([&endpoint]()
                {
                    int ping;
                    _windows_ping(endpoint._ping_ip, &ping, 2000);

                    if (ping > 0)
                        endpoint.ping = ping + ping_offset;
                }).detach();

            }
            
            //std::this_thread::sleep_for(std::chrono::milliseconds(interval));

            std::unique_lock<std::mutex> lk(cv_m);
            if (cv.wait_for(lk, 1 * std::chrono::milliseconds(interval), [&] { return this->pinging == false; }))
            {
                //printf(std::format("ep1 loaded in time with ping {0}\n", pinging ? "true" : "false").c_str());
            }
            else
            {
                //printf(std::format("ep1 timed out before loading ping {0}\n", pinging ? "true" : "false").c_str());
            }
        }

        printf("done pinging\n");

    });

    thread.detach();


}

DashboardManager::~DashboardManager()
{
    this->pinging = false;
    //this->pinging->store(false);
}


/*
    current way i'm getting ips:
        > tracert any edge ip
        > get router ip, 2nd from last?
*/
DashboardManager::DashboardManager() : 
    
    ips({
        // USA - West
        { "lax1", "24.105.8.0-24.105.15.255,34.124.0.0/21" },

        // USA - Central
        { "ord1", "24.105.40.0-24.105.47.255,8.34.210.0/24,8.34.212.0/22,8.34.216.0/22,8.35.192.0/21,23.236.48.0/20,23.251.144.0/20,34.16.0.0/17,34.27.0.0/16,34.28.0.0/14,34.66.0.0/15,34.68.0.0/14,34.72.0.0/16,34.118.200.0/21,34.121.0.0/16,34.122.0.0/15,34.132.0.0/14,34.136.0.0/16,34.157.84.0/23,34.157.96.0/20,34.157.212.0/23,34.157.224.0/20,34.170.0.0/15,34.172.0.0/15,35.184.0.0/16,35.188.0.0/17,35.188.128.0/18,35.188.192.0/19,35.192.0.0/15,35.194.0.0/18,35.202.0.0/16,35.206.64.0/18,35.208.0.0/15,35.220.64.0/19,35.222.0.0/15,35.224.0.0/15,35.226.0.0/16,35.232.0.0/16,35.238.0.0/15,35.242.96.0/19,104.154.16.0/20,104.154.32.0/19,104.154.64.0/19,104.154.96.0/20,104.154.113.0/24,104.154.114.0/23,104.154.116.0/22,104.154.120.0/23,104.154.128.0/17,104.155.128.0/18,104.197.0.0/16,104.198.16.0/20,104.198.32.0/19,104.198.64.0/20,104.198.128.0/17,107.178.208.0/20,108.59.80.0/21,130.211.112.0/20,130.211.128.0/18,130.211.192.0/19,130.211.224.0/20,146.148.32.0/19,146.148.64.0/19,146.148.96.0/20,162.222.176.0/21,173.255.112.0/21,199.192.115.0/24,199.223.232.0/22,199.223.236.0/24,34.22.0.0/19,35.186.0.0/17,35.186.128.0/20,35.206.32.0/19,35.220.46.0/24,35.242.46.0/24,107.167.160.0/20,108.59.88.0/21,173.255.120.0/21" },

        // USA - West 2
        { "guw2", "35.247.0.0/17,35.236.0.0/17,35.235.64.0/18,34.102.0.0/17,34.94.0.0/16,34.19.0.0/17,34.82.0.0/15,34.105.0.0/17,34.118.192.0/21,34.127.0.0/17,34.145.0.0/17,34.157.112.0/21,34.157.240.0/21,34.168.0.0/15,35.185.192.0/18,35.197.0.0/17,35.199.144.0/20,35.199.160.0/19,35.203.128.0/18,35.212.128.0/17,35.220.48.0/21,35.227.128.0/18,35.230.0.0/17,35.233.128.0/17,35.242.48.0/21,35.243.32.0/21,35.247.0.0/17,104.196.224.0/19,104.198.0.0/20,104.198.96.0/20,104.199.112.0/20,34.20.128.0/17,34.94.0.0/16,34.102.0.0/17,34.104.64.0/21,34.108.0.0/16,34.118.248.0/23,35.215.64.0/18,35.220.47.0/24,35.235.64.0/18,35.236.0.0/17,35.242.47.0/24,35.243.0.0/21,34.22.32.0/19,34.104.52.0/24,34.106.0.0/16,34.127.180.0/24,35.217.64.0/18,35.220.31.0/24,35.242.31.0/24,34.16.128.0/17,34.104.72.0/22,34.118.240.0/22,34.124.8.0/22,34.125.0.0/16,35.219.128.0/18,34.124.0.0/21" },

        // USA - East 2
        { "gue4", "104.196.0.0/18,104.196.128.0/18,104.196.192.0/19,104.196.65.0/24,104.196.66.0/23,104.196.68.0/22,104.196.96.0/19,162.216.148.0/22,34.104.124.0/23,34.104.56.0/23,34.104.60.0/23,34.118.250.0/23,34.118.252.0/23,34.124.60.0/23,34.127.184.0/23,34.127.188.0/23,34.138.0.0/15,34.145.128.0-34.145.255.255,34.145.128.0/17,34.148.0.0/16,34.150.128.0-34.150.255.255,34.150.128.0/17,34.157.0.0/21,34.157.128.0/21,34.157.144.0/20,34.157.16.0/20,34.157.160.0/22,34.157.32.0/22,34.161.0.0/16,34.162.0.0/16,34.21.0.0/17,34.23.0.0/16,34.24.0.0/15,34.26.0.0/16,34.73.0.0/16,34.74.0.0/15,34.85.128.0-34.85.255.255,34.85.128.0/17,34.86.0.0-34.86.255.255,34.86.0.0/16,34.98.128.0/21,35.185.0.0/17,35.186.160.0-35.186.191.255,35.186.160.0/19,35.188.224.0/19,35.190.128.0/18,35.194.64.0/19,35.196.0.0/16,35.199.0.0-35.199.63.255,35.199.0.0/18,35.206.10.0/23,35.207.0.0/18,35.211.0.0/16,35.212.0.0/17,35.220.0.0/20,35.220.60.0/22,35.221.0.0/18,35.227.0.0/17,35.229.16.0/20,35.229.32.0/19,35.229.64.0/18,35.230.160.0/19,35.231.0.0/16,35.234.176.0/20,35.236.192.0-35.236.255.255,35.236.192.0/18,35.237.0.0/16,35.242.0.0/20,35.242.60.0/22,35.243.128.0/17,35.243.40.0/21,35.245.0.0-35.245.255.255,35.245.0.0/16,34.152.72.0/21,34.177.40.0/21" },

        // Netherlands
        { "ams1", "5.42.168.0-5.42.175.255,64.224.26.0/23" },

        // France
        { "cdg1", "5.42.184.0-5.42.191.255" },

        // Finland
        { "gen1", "34.88.0.0/16,34.104.96.0/21,34.124.32.0/21,35.203.232.0/21,35.217.0.0/18,35.220.26.0/24,35.228.0.0/16,35.242.26.0/24" },
        
        // Brazil 2
        { "gbr1", "34.95.128.0/17,34.104.80.0/21,34.124.16.0/21,34.151.0.0/18,34.151.192.0/18,35.198.0.0/18,35.199.64.0/18,35.215.192.0/18,35.220.40.0/24,35.235.0.0/20,35.242.40.0/24,35.247.192.0/18,34.104.50.0/23,34.127.178.0/23,34.176.0.0/16" },

        // Japan 2
        { "gtk1", "34.85.0.0-34.85.127.255,34.84.0.0-34.84.255.255,35.190.224.0-35.190.239.255,35.194.96.0-35.194.255.255,35.221.64.0-35.221.255.255,34.146.0.0-34.146.255.255,34.84.0.0/16,34.85.0.0/17,34.104.62.0/23,34.104.128.0/17,34.127.190.0/23,34.146.0.0/16,34.157.64.0/20,34.157.164.0/22,34.157.192.0/20,35.187.192.0/19,35.189.128.0/19,35.190.224.0/20,35.194.96.0/19,35.200.0.0/17,35.213.0.0/17,35.220.56.0/22,35.221.64.0/18,35.230.240.0/20,35.242.56.0/22,35.243.64.0/18,104.198.80.0/20,104.198.112.0/20,34.97.0.0/16,34.104.49.0/24,34.127.177.0/24,35.217.128.0/17,35.220.45.0/24,35.242.45.0/24,35.243.56.0/21" },

        // Singapore 2
        { "gsg1", "34.124.0.0-34.124.255.255,34.124.42.0-34.124.43.255,34.142.128.0-34.142.255.255,35.185.176.0-35.185.191.255,35.186.144.0-35.186.159.255,35.247.128.0-35.247.191.255,34.87.0.0-34.87.191.255,34.143.128.0-34.143.255.255,34.124.128.0-34.124.255.255,34.126.64.0-34.126.191.255,35.240.128.0-35.240.255.255,35.198.192.0-35.198.255.255,34.21.128.0-34.21.255.255,34.104.58.0-34.104.59.255,34.124.41.0-34.124.42.255,34.157.82.0-34.157.83.255,34.157.88.0-34.157.89.255,34.157.210.0-34.157.211.255,35.187.224.0-35.187.255.255,35.197.128.0-35.197.159.255,35.213.128.0-35.213.191.255,35.220.24.0-35.220.25.255,35.234.192.0-35.234.207.255,35.242.24.0-35.242.25.255,34.126.128.0/18,34.87.128.0/18,34.21.128.0/17,34.87.0.0/17,34.87.128.0/18,34.104.58.0/23,34.104.106.0/23,34.124.42.0/23,34.124.128.0/17,34.126.64.0/18,34.126.128.0/18,34.142.128.0/17,34.143.128.0/17,34.157.82.0/23,34.157.88.0/23,34.157.210.0/23,35.185.176.0/20,35.186.144.0/20,35.187.224.0/19,35.197.128.0/19,35.198.192.0/18,35.213.128.0/18,35.220.24.0/23,35.234.192.0/20,35.240.128.0/17,35.242.24.0/23,35.247.128.0/18,34.101.18.0/24,34.101.20.0/22,34.101.24.0/22,34.101.32.0/19,34.101.64.0/18,34.101.128.0/17,34.128.64.0/18,35.219.0.0/17" },
        
        // South Korea
        { "icn1", "121.254.0.0-121.254.255.255,117.52.0.0-117.52.255.255,202.9.66.0/23" },
        
        // Taiwan
        { "tpe1", "5.42.160.0-5.42.160.255,35.221.128.0/17" },
        
        // Dubai
        { "gmec1", "34.1.32.0/20,34.18.0.0/16,34.157.126.0/23,34.157.252.0/23" },
        
        // Australia 3
        { "syd2", "158.115.196.0/23,37.244.42.0-37.244.42.255,34.87.192.0/18,34.104.104.0/23,34.116.64.0/18,34.124.40.0/23,34.151.64.0/18,34.151.128.0/18,35.189.0.0/18,35.197.160.0/19,35.201.0.0/19,35.213.192.0/18,35.220.41.0/24,35.234.224.0/20,35.242.41.0/24,35.244.64.0/18,34.104.122.0/23,34.124.58.0/23,34.126.192.0/20,34.129.0.0/16,34.0.16.0/20" },

    }),
    
    endpoints({
        /*js
            document.body.innerText.replaceAll('\n', ',');
        */

        // https://ipinfo.io/AS57976/137.221.68.0/24
        {
            .title=                   "USA - West",
            ._ping_ip=                "137.221.68.83",
            .heading=                 "LAX1 and GUW2",
            ._firewall_rule_address=  ips.at("lax1") + "," + ips.at("guw2"),
            ._firewall_rule_description =             "Blocks LAX1 and GUW2",
            .favorite=                true,
        },

        // https://ipinfo.io/AS57976/137.221.69.0/24
        {
            .title=                   "USA - Central",
            ._ping_ip=                "137.221.69.29",
            .heading=                 "ORD1",
            ._firewall_rule_address=  ips.at("ord1"),
            ._firewall_rule_description =             "Blocks ORD1",
            .favorite = true,
        },

        // https://ipinfo.io/AS57976/137.221.78.0/24
        {
            .title = "Netherlands",
            ._ping_ip = "137.221.78.69",
            .heading = "AMS1",
            ._firewall_rule_address = ips.at("ams1"),
            ._firewall_rule_description = "Blocks AMS1",
        },

        // https://ipinfo.io/AS57976/137.221.77.0/24
        {
            .title = "France",
            ._ping_ip = "137.221.77.51",
            .heading = "CDG1",
            ._firewall_rule_address = ips.at("cdg1"),
            ._firewall_rule_description = "Blocks CDG1",
        },

        // blizz hidden
        {
            .title = "Finland",
            ._ping_ip = "188.126.89.1",
            .heading = "GEN1",
            ._firewall_rule_address = ips.at("gen1"),
            ._firewall_rule_description = "Blocks GEN1",
        },

        // blizz hidden
        {
            .title = "Brazil",
            ._ping_ip = "52.94.7.202",
            .heading = "GBR1",
            ._firewall_rule_address = ips.at("gbr1"),
            ._firewall_rule_description = "Blocks GBR1",
        },

        // blizz hidden
        {
            .title = "Japan",
            ._ping_ip = "52.94.8.94",
            .heading = "GTK1",
            ._firewall_rule_address = ips.at("gtk1"),
            ._firewall_rule_description = "Blocks GTK1",
        },

        // blizz hidden
        {
            .title = "Singapore",
            ._ping_ip = "52.94.11.146",
            .heading = "GSG1",
            ._firewall_rule_address = ips.at("gsg1"),
            ._firewall_rule_description = "Blocks GSG1",
        },

        // https://ipinfo.io/AS57976/137.221.64.0/19-137.221.65.0/25
        {
            .title = "South Korea",
            ._ping_ip = "137.221.65.65",
            .heading = "ICN1",
            ._firewall_rule_address = ips.at("icn1"),
            ._firewall_rule_description = "Blocks ICN1",
        },

        // https://ipinfo.io/AS57976/137.221.64.0/19-137.221.67.0/25
        {
            .title = "Taiwan",
            ._ping_ip = "137.221.112.69",
            .heading = "TPE1",
            ._firewall_rule_address = ips.at("tpe1"),
            ._firewall_rule_description = "Blocks TPE1",
        },

        // blizz hidden
        {
            .title = "Dubai",
            ._ping_ip = "13.248.66.130",
            .heading = "GMEC1",
            ._firewall_rule_address = ips.at("gmec1"),
            ._firewall_rule_description = "Blocks GMEC1",
        },

        // https://ipinfo.io/AS57976/137.221.85.0/24
        {
            .title = "Australia",
            ._ping_ip = "137.221.85.67",
            .heading = "SYD2",
            ._firewall_rule_address = ips.at("syd2"),
            ._firewall_rule_description = "Blocks SYD2",
        },

        // TODO: germany, bahrain. offline currently.
    }),

    // endpoints({

    //{ "USA - EAST", "", "gue1"},
    //{ "USA - CENTRAL", "24.105.62.129" },
    // ord1, chicago


    /*
    std::make_shared<Endpoint>("NETHERLANDS", "9.9.9.9", "ord1"),
    std::make_shared<Endpoint>("FRANCE", "9.9.9.9 - 9.9.9.9 (+4)", "ord1"),
    std::make_shared<Endpoint>("BRAZIL 2", "9.9.9.9", "ord1"),
    std::make_shared<Endpoint>("FINLAND 2", "9.9.9.9", "ord1"),
    std::make_shared<Endpoint>("SINGAPORE 2", "9.9.9.9", "ord1"),
    std::make_shared<Endpoint>("JAPAN 2", "9.9.9.9", "ord1"),
    std::make_shared<Endpoint>("SOUTH KOREA", "211.234.110.1", ""),
    std::make_shared<Endpoint>("USA - WEST", "24.105.30.129", "lax1"), // lax-eqla1-ia-bons-03.as57976.net
    std::make_shared<Endpoint>("USA - CENTRAL", "24.105.62.129", "ord1"),
    std::make_shared<Endpoint>("AUSTRALIA 3", "9.9.9.9", "ord1"),
    std::make_shared<Endpoint>("TAIWAN", "9.9.9.9", "ord1"),
    */

    // std::make_shared<Endpoint>("USA - WEST", "24.105.30.129", "lax1"), // https://whatismyipaddress.com/ip/24.105.30.129
    //std::make_shared<Endpoint>("USA - CENTRAL", "24.105.62.129", "ord1"),

    /*js
        document.body.innerText.replaceAll('\n', ',');
    */

    // https://ipinfo.io/AS57976/137.221.68.0/24
    // std::make_shared<Endpoint>(
    //     /*.title=*/                   "USA - WEST",
    //     /*.hostname=*/                "137.221.68.83",
    //     /*.heading=*/                 "137.221.68.0/24",
    //     /*._firewall_rule_address=*/  ips.at("lax1") + "," + ips.at("guw2"),
    //     /*.description=*/             "Blocks LAX1 and GUW2"
    // ),

    // ord1, 
    // https://ipinfo.io/AS57976/137.221.69.0/24
    // std::make_shared<Endpoint>(
    //     /*.title=*/                   "USA - CENTRAL",
    //     /*.hostname=*/                "137.221.69.29",
    //     /*.heading=*/                 "137.221.69.0/24",
    //     /*._firewall_rule_address=*/  ips.at("ord1"),
    //     /*.description=*/             "Blocks ORD1"
    // ),


    // these two are same
    //{ "west", "lax-eqla1-ia-bons-03.as57976.net" },

    //{ "‚¢‚í‚«Žs", "dynamodb.ap-northeast-1.amazonaws.com" },
    //{ "JAPAN 2", "dynamodb.ap-northeast-1.amazonaws.com" },
    //std::make_shared<Endpoint>("JAPAN 2", "dynamodb.ap-northeast-1.amazonaws.com"),
    //std::make_shared<Endpoint>("TAIWAN", "203.66.81.98"),
    //std::make_shared<Endpoint>("GERMANY", "127.0.0.1", "gew3"),

    // }),
    pinging(true)
    //pinging(std::make_shared<std::atomic_bool>(true))
    //pinging(std::make_shared<bool>(true))
{

    //bool done = loadPicture("cover_browser", "jpg", &cover_texture.texture, &cover_texture.width, &cover_texture.height);

    Process p = {
        .pid = 0,
        .on = false,
        .icon = { ImageTexture{ nullptr, 0, 0 } },
        .window = 0,
    };

    this->processes["Overwatch.exe"] = p;

    std::thread([&]()
        {

            // wait until imgui is loaded
            while (ImGui::GetCurrentContext() == nullptr)
                std::this_thread::sleep_for(std::chrono::milliseconds(16));

            while (true)
            {

                {

                    bool __previous__application_open = appStore.application_open;

                    static const std::string process_name = "Overwatch.exe";
                    static const std::string module_name = "Overwatch.exe";

                    int pid = find_process (process_name);
                    HWND window = find_window ("Overwatch");

                    appStore.application_open = window;
                    this->processes["Overwatch.exe"].on = window;
                    this->processes["Overwatch.exe"].window = window;

                    if (__previous__application_open && !appStore.application_open)
                    {
                        firewallManager.sync(&(this->endpoints));
                    }

                    // std::cout << window << std::endl;


                    {
                        if (this->processes[process_name].icon.texture == nullptr)
                        {

                            // TODO
                            // try to extract png icon from exe again. this was really hard, find a library if possible.

                            //std::vector<unsigned char> buffer;
                            //ZeroMemory(&bmp, sizeof(bmp));

                            /*if (get_module_icon(pid, module_name, &buffer))
                            {
                                bool worked = _loadPicture(buffer.data(), sizeof(buffer.data()), &(this->_other_party_app_icon_0.texture), &(this->_other_party_app_icon_0.width), &(this->_other_party_app_icon_0.height));
                                printf(worked ? "image loaded\n" : "image failed\n");
                            }*/


                            // just prints stuff for now
                            get_module(pid, module_name);
                            
                            loadPicture("process_icon_overwatch", "png", &(this->processes[process_name].icon.texture), &(this->processes[process_name].icon.width), &(this->processes[process_name].icon.height));

                        }
                    }

                    /*if (pid)
                        printf("PID = %d\n", pid);
                    else
                        printf("(process) no pid\n");*/


                    /*if (window)
                        printf("window found\n");
                    else
                        printf("no window\n");

                    this->processes[process_name].on = window;
                    this->processes[process_name].window = window;*/
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
        }).detach();

    //::global_message = done ? "done cover_browser.jpg" : "D: failed.";

    {
        // 1) load rules from wf.msc
        firewallManager._syncFirewallWithEndpoints(&(this->endpoints));

        // 2) reset all rules
        firewallManager.flushRules(&(this->endpoints));

        // 3) sync with endpoints
        firewallManager._syncEndpointsWithFirewall(&(this->endpoints));
    }
}

void DashboardManager::loadAssets() {


    static const std::vector<std::string> textures = {
        "icon_options.png",
        "icon_maple_leaf.png",
        "icon_chain_slash.png",
        "icon_bolt.png",
        "icon_outside_window.png",

        "icon_wifi_slash.png",
        "icon_wifi_poor.png",
        "icon_wifi_fair.png",
        "icon_wifi.png",

        "icon_allow.png",
        "icon_block.png",
        "icon_wall_fire.png",

        //"icon_arrow.png",
        "icon_angle.png",

        "background_app.png",
        "background_diagonal.png",
    };

    // load textures
    {
        std::thread([]()
        {
            // std::cout << "<" << std::hex << std::this_thread::get_id() << "> textures//dashboard" << std::endl;
            for (std::string texture : textures)
            {
                std::string title = texture.substr(0, texture.find("."));
                std::string type = texture.substr(texture.find(".") + 1);
                const auto loaded = _add_texture(title, type);
                // std::cout << "<" << std::hex << std::this_thread::get_id() << ">  .. \"" << name << "." << type << "\" " << (loaded ? "" : "(fail)") << std::endl;
            }
            std::cout << "<" << std::hex << std::this_thread::get_id() << "> loaded " << std::dec << textures.size() << " textures." << std::endl;
            //std::cout << std::endl;
        }).detach();
    }
}


void DashboardManager::RenderInline(/* bool* p_open */)
{



    if (ImGui::GetCurrentContext() != nullptr && ImGui::IsWindowAppearing())
    {
        this->loadAssets();
    }

    {
        /*if (!ImGui::IsWindowAppearing() && (!this->active && !ImGui::IsPopupOpen("deactivated")))
            ImGui::OpenPopup("deactivated");

        static const auto transparent = ImGui::ColorConvertFloat4ToU32({ 0, 0, 0, 0 });
        ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, transparent);
        if (ImGui::BeginPopupModal("deactivated", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground))
        {
            ImGui::TextColored(ImColor::HSV(0, .2, 1), "deactivated");
            ToggleButton("test", &(this->active));
            ImGui::EndPopup();
        }
        ImGui::PopStyleColor(1);*/
    }

    if (ImGui::GetFrameCount() == 8)
    {
        
    }

    {

        // makeshift initial sorting
        //if (ImGui::GetFrameCount() == 20)
            //ImGui

        // move the numbers around slightly in between pings >:p
        // tricky tricky
        if (this->pinging && (ImGui::GetFrameCount() % 200) == 0)
        {
            const int max = 1;
            const int min = -2;

            for (auto& endpoint : endpoints)
            {
                if (endpoint.display_ping != endpoint.ping)
                    continue;

                if (endpoint.ping < 0)
                    continue;

                const int range = max - min + 1;
                const int num = rand() % range + min;

                //endpoint.display_ping += num;
                endpoint.display_ping = std::max(0, endpoint.display_ping + num);
            }
        }

        for (auto& endpoint : endpoints)
        {
            if (endpoint.display_ping != endpoint.ping)
            {

                if (endpoint.ping > 0 && endpoint.display_ping <= 0)
                {
                    endpoint.display_ping = endpoint.ping;
                    continue;
                }

                const int diff = std::abs(endpoint.ping - endpoint.display_ping);


                // 90 is domain of diff
                float param = fmin((float)diff / 64.0f, 1.0f);


                const float min_delay = 1.0f;
                const float max_delay = 15.0f;


                if (ImGui::GetFrameCount() % (int)fmax(min_delay, max_delay - (max_delay * param * half_pi)) != 0)
                    continue;

                if (endpoint.display_ping < endpoint.ping)
                {
                    endpoint.display_ping ++;
                }
                else
                {
                    endpoint.display_ping --;
                }

                endpoint.display_ping = std::max(0, endpoint.display_ping);
            }
        }
    }

    //TODO transparency
    //if (ImGui::IsWindowHovered) pushstyle alpha .9f

    {
        /*ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoScrollbar;
        ImGui::SetNextWindowSize(ImVec2(418, 450), ImGuiCond_Once);
        //ImGui::SetNextWindowSize(ImVec2(418, 0));
        //ImGui::SetNextWindowPos(ImVec2(1920 - 400 - 90, 150), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(1920 - 400 - 90, 150), ImGuiCond_Once);
        // ImGui::Begin("dashboard", p_open, window_flags);

        ImGui::Begin("dashboard", &(this->window_open), window_flags);*/

        if (ImGui::IsWindowAppearing())
            this->startPinging();

        static auto &style = ImGui::GetStyle();
        ImU32 const white = ImGui::ColorConvertFloat4ToU32({ 1, 1, 1, style.Alpha });
        //ImU32 const white = _UI32_MAX;

        ImVec2 const windowPos = ImGui::GetWindowPos();
        ImVec2 const windowSize = ImGui::GetWindowSize();

        ImDrawList* list = ImGui::GetWindowDrawList();
        ImDrawList* bg_list = ImGui::GetBackgroundDrawList();
        //ImDrawList* fg_list = ImGui::GetForegroundDrawList();
        float const scrollY = ImGui::GetScrollY();

        const auto color_button = ImColor::HSV(0, 0.4f, 1, style.Alpha);
        const auto color_button_hover = ImColor::HSV(0, 0.25f, 1, style.Alpha);

        static const ImU32 color_text = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);
        const ImU32 color_text_secondary = ImGui::ColorConvertFloat4ToU32({ 1, 1, 1, .8f * style.Alpha });

        /*if (this->__date_new_selection != 0)
        {

            static const unsigned char delay = 4;

            if ((ImGui::GetTime() - this->__date_new_selection) > delay)
            {
                this->__date_new_selection = 0;
                firewallManager.sync(&(this->endpoints));
            }
        }*/

        // background texture
        {
            // TODO RED IF ERROR
            //const auto color = ImColor::HSV((ImGui::GetFrameCount() % 600) / 600.0f, .2, 1, style.Alpha);

            bg_list->AddRectFilled(windowPos, windowPos + ImGui::GetWindowSize(), white, 9);
            bg_list->AddImageRounded(_get_texture("background_app"), windowPos - ImVec2(0, ImGui::GetScrollY() / 4), windowPos - ImVec2(0, ImGui::GetScrollY() / 4) + ImVec2(ImGui::GetWindowSize().x, 430), ImVec2(0, 0), ImVec2(1, 1), white, 9);
        }

        ImGui::Spacing();

        // top part
        {
            auto const size = 24;
            const auto widgetPos = ImGui::GetCursorScreenPos();

            
            ImGui::BeginGroup();
            {
                // ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
                // version
                //list->AddText(NULL, 14, windowPos + widgetPos, (ImColor) ImVec4({ .8, .8, .8, 1 }), "v0.0");
                //ImGui::Dummy({ 0, 0 });

                // title
                /*ImGui::PushFont(font_title);
                ImGui::PushStyleColor(ImGuiCol_Text, this->title_color);
                ImGui::Text("DASHBOARD // title");
                ImGui::PopStyleColor();
                ImGui::PopFont();*/

                list->AddText(font_title, font_title->FontSize, widgetPos - ImVec2(1, 0), color_text, appStore.dashboard.title.c_str());
                ImGui::Dummy({ 0, font_title->FontSize - 6 });

                // subtitle
                ImGui::TextWrapped(appStore.dashboard.heading.c_str());

                // ImGui::PopStyleVar();
            }
            ImGui::EndGroup();

            ImGui::SameLine();
            ImGui::Dummy({ ImGui::GetContentRegionAvail().x - size - 4 - size - 12, size });
            ImGui::SameLine(NULL, 0);

            const auto offset = ImVec2(0, 10);
            ImGui::SetCursorPos(ImGui::GetCursorPos() + offset);

            //auto const hovered = ImGui::IsMouseHoveringRect({ pos.x, pos.y }, { pos.x + size.x, pos.y + size.y });

            // socials
            {
                ImGui::Dummy({ size, size });
                list->AddImage((void*) _get_texture("icon_bolt"), ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), ImGui::IsItemHovered() ? color_button_hover : color_button);

                if (ImGui::IsItemClicked())
                    ImGui::OpenPopup("socials");
            }

            ImGui::SameLine(NULL, 10);
            ImGui::SetCursorPos(ImGui::GetCursorPos() + offset);

            // options
            {
                ImGui::Dummy({ size, size });
                list->AddImage((void*) _get_texture("icon_options"), ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), ImGui::IsItemHovered() ? color_button_hover : color_button);

                if (ImGui::IsItemClicked())
                    ImGui::OpenPopup("options");
            }
        }

        // ImGui::Spacing();

        for (auto const& [_p, process] : this->processes)
        {

            // TODO current window
            if (appStore._window_overlaying == "Overwatch")
                continue;

            ImGui::BeginGroup();
            {
                if (!process.on)
                    ImGui::BeginDisabled();

                {
                    ImGui::Dummy({ 184, 40 });

                    const auto hovered = ImGui::IsItemHovered();

                    const auto p_min = ImGui::GetItemRectMin();
                    const auto p_max = ImGui::GetItemRectMax();

                    static const auto frame = ImVec2(24, 24);

                    static const auto color = ImGui::ColorConvertFloat4ToU32({ 1, 1, 1, 0.6f });
                    static const auto color_2 = ImGui::ColorConvertFloat4ToU32({ 1, 1, 1, 0.4f });
                    static const auto text_color_2 = ImGui::ColorConvertFloat4ToU32({ 0, 0, 0, 0.4f * style.Alpha });
                    static const auto icon_color_2 = ImGui::ColorConvertFloat4ToU32({ 1, 1, 1, 0.4f * style.Alpha });


                    list->AddRectFilled(p_min, p_max, hovered ? white : (!process.on ? color_2 : color), 9);
                    if (hovered)
                        list->AddRect(p_min, p_max, white, 14, NULL, 4);

                    const auto frame_pos = p_min + ImVec2(8, 8);
                    list->AddImage((void*)process.icon.texture, frame_pos, frame_pos + frame, ImVec2(0, 0), ImVec2(1, 1), !process.on ? icon_color_2 : white);

                    const auto text_pos = p_min + ImVec2(frame.x + 16, 6);
                    list->AddText(text_pos, !process.on ? text_color_2 : color_text, "overwatch.exe");
                }

                if(!process.on)
                    ImGui::EndDisabled();
            }
            ImGui::EndGroup();

            if (ImGui::IsItemClicked() && process.window)
            {
                // focus window
                SetForegroundWindow(process.window);

                // maximize window
                PostMessage(process.window, WM_SYSCOMMAND, SC_RESTORE, 0);
            }
        }        

        ImGui::Spacing();

        //ImGui::Dummy({ 0, 20 });

        {
            ImGui::BeginChild("endpoints_scrollable", ImVec2(ImGui::GetContentRegionAvail().x, 540), false);

            {
                int i = 0;
                for (auto& endpoint : this->endpoints)
                {
                    ImU32 const color = ImColor::HSV(i / 14.0f, 0.4f, 1.0f, style.Alpha);
                    ImU32 const color_secondary = ImColor::HSV(i / 14.0f, 0.3f, 1.0f, style.Alpha);
                    ImU32 const color_secondary_faded = ImColor::HSV(i / 14.0f, 0.2f, 1.0f, 0.4f * style.Alpha);

                    auto const w_list = ImGui::GetWindowDrawList();

                    auto const disabled = !(endpoint.ping > 0);
                    auto const &selected = endpoint.selected;

                    ImGui::Dummy({ 0 ,0 });
                    ImGui::SameLine(NULL, 16);
                    ImGui::Dummy({ ImGui::GetContentRegionAvail().x - 16, 73 });

                    auto hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup);

                    // background
                    if (hovered)
                    {
                        if (selected)
                        {
                            w_list->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), color_secondary, 5, 0, 8);
                            w_list->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), color_secondary, 5, NULL);
                        }
                        else
                        {
                            w_list->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), color_secondary_faded, 5, NULL);
                        }
                    }
                    else
                    {
                        if (selected)
                        {
                            w_list->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), color, 5, NULL);
                        }
                    }

                    // unsynced background
                    if (endpoint.unsynced)
                    {
                        static const auto color = color_secondary_faded;

                        const auto offset = (ImGui::GetFrameCount() / 4) % 40;

                        const auto offset_vec = ImVec2(offset, offset);

                        const auto pos = ImGui::GetItemRectMin() - ImVec2(40, 40) + offset_vec;

                        w_list->PushClipRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), true);
                        w_list->AddImage(_get_texture("background_diagonal"), pos, pos + ImVec2(400, 400), ImVec2(0, 0), ImVec2(1, 1), color);
                        w_list->PopClipRect();
                    }

                    // icon
                    const auto icon_frame = ImVec2({ ImGui::GetItemRectSize().y, ImGui::GetItemRectSize().y });

                    static auto padding = ImVec2(21, 21);

                    const auto icon = !endpoint.unsynced ? (selected ? _get_texture("icon_allow") : _get_texture("icon_block")) : _get_texture("icon_wall_fire");

                    w_list->AddImage(icon, ImGui::GetItemRectMin() + padding, ImGui::GetItemRectMin() + icon_frame - padding, ImVec2(0, 0), ImVec2(1, 1), selected ? color_text_secondary : color /*color_secondary_faded*/);

                    // display 1
                    //auto pos = ImGui::GetItemRectMin() + style.FramePadding - ImVec2(0, 4);
                    auto pos = ImGui::GetItemRectMin() + ImVec2(icon_frame.x, 4);
                    w_list->AddText(font_title, 35, pos, selected ? white : color, endpoint.title.c_str());

                    // display 2
                    pos += ImVec2(2, ImGui::GetItemRectSize().y - 24 - 14);
                    //list->AddText(font_subtitle, 24, pos, selected ? color_text_secondary : color_secondary, !hovered ? endpoint.description.c_str() : endpoint.heading.c_str());
                    w_list->AddText(font_subtitle, 24, pos, selected ? color_text_secondary : color_secondary, endpoint.heading.c_str());

                    // popup
                    /*{
                        std::string key = std::format("endpoint{0}", i);
                        if (hovered || ImGui::IsPopupOpen(key.c_str()))
                            if (ImGui::BeginPopupContextItem(key.c_str()))
                            {
                                {
                                    ImGui::MenuItem(endpoint->name.c_str(), NULL, false, false);
                                    if (ImGui::MenuItem("disable")) {}
                                    if (ImGui::MenuItem("block", "wip")) {}

                                    //ImGui::SeparatorText("xx");
                                }

                                if (!hovered && !ImGui::IsWindowHovered())
                                    ImGui::CloseCurrentPopup();

                                ImGui::EndPopup();
                            }

                        ImGui::OpenPopupOnItemClick(key.c_str());
                    }*/

                    // action
                    if (ImGui::IsItemClicked())
                    {
                        int total = 0;
                        for (auto &endpoint : this->endpoints)
                            if (endpoint.selected)
                                total++;

                        if (total > 1 || !endpoint.selected)
                            endpoint.selected = !endpoint.selected;

                        endpoint.unsynced = true;
                        this->__date_new_selection = ImGui::GetTime();

                        // TODO if only unblocked, no need to wait for application restart
                        if (!appStore.application_open)
                            firewallManager.sync(&(this->endpoints));
                    }

                    // display 3 / (icon wifi)
                    {
                        auto icon = _get_texture("icon_wifi_slash");
                        static ImVec2 frame = ImVec2(26, 26);

                        if (0 < endpoint.ping)
                        {
                            if (endpoint.display_ping > 120)
                                icon = _get_texture("icon_wifi_poor");
                            else if (endpoint.display_ping > 60)
                                icon = _get_texture("icon_wifi_fair");
                            else
                                icon = _get_texture("icon_wifi");
                        }

                        auto pos = ImGui::GetItemRectMax() - ImVec2(frame.x, ImGui::GetItemRectSize().y) + (style.FramePadding * ImVec2(-1, 1)) + ImVec2(-4, 1);
                        w_list->AddImage(icon, pos, pos + frame, ImVec2(0, 0), ImVec2(1, 1), selected ? white : color);

                    }

                    // display 4
                    {
                        auto const text = std::to_string(endpoint.display_ping);

                        auto text_size = font_subtitle->CalcTextSizeA(24, FLT_MAX, 0.0f, text.c_str());
                        auto pos = ImGui::GetItemRectMax() - style.FramePadding - text_size + ImVec2(-4, 0);

                        w_list->AddText(font_subtitle, 24, pos, selected ? color_text_secondary : color_secondary, text.c_str());
                    }

                    i += 1;
                }
            }

            ImGui::EndChild();

            //ImGui::TextWrapped("Changes will be applied after closing the game");


            // bottom gap
            /*if (!this->show_all)
                ImGui::Dummy({ 0, ImGui::GetContentRegionAvail().y - 48 - 10 });*/

            ImGui::Spacing();

            // bottom part
            {
                ImGui::Dummy({ ImGui::GetContentRegionAvail().x, ImGui::GetFont()->FontSize + (style.FramePadding.y * 2) });
                list->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), white, 5);
                list->AddText(ImGui::GetItemRectMin() + style.FramePadding + ImVec2(2, 0), color_text, "Advanced");

                static ImVec2 frame = ImVec2(24, 24);
                auto const pos = ImVec2(ImGui::GetItemRectMax() - frame - ImVec2(style.FramePadding.x + 4, 14));
                auto const uv_min = !this->show_all ? ImVec2(0, 0) : ImVec2(0, 1);
                auto const uv_max = !this->show_all ? ImVec2(1, 1) : ImVec2(1, 0);
                list->AddImage(_get_texture("icon_angle"), pos, pos + frame, uv_min, uv_max, color_text);

                if (ImGui::IsItemClicked())
                {
                    this->show_all = !this->show_all;
                }

                if (this->show_all)
                {
                    //ImGui::BeginChild("#show_all");
                    ImGui::BeginGroup();
                    ImGui::Indent(style.FramePadding.x);
                    {
                        ImGui::Text("Unavailable in this version.");
                        static bool test;
                        ToggleButton("test", &test);
                    }
                    ImGui::Unindent();
                    ImGui::EndGroup();
                    //list->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), (ImU32)ImGui::ColorConvertFloat4ToU32({ 1, 1, 1, 1 }));
                    //ImGui::EndChild();
                    
                }
            }

            //ImGui::EndChild();
        }
        //ImGui::EndChild();

        //ImGui::Button("apply", { ImGui::GetContentRegionAvail().x, 32 });


        {
            if (ImGui::BeginPopupModal("options", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoResize))
            {
                ImDrawList* list = ImGui::GetWindowDrawList();

                ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

                // handle close
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowHovered() && !ImGui::IsWindowAppearing())
                    ImGui::CloseCurrentPopup();

                /*ImGui::SetWindowFontScale(1.4f);
                ImGui::MenuItem("options", NULL, false, false);
                ImGui::SetWindowFontScale(1.0f);*/

                // title
                ImGui::PushFont(font_title);
                ImGui::Text("OPTIONS");
                ImGui::PopFont();

                //list->AddRectFilled(ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax(), ImGui::ColorConvertFloat4ToU32({ 1, 0, 0, .4f }));
                //ImGui::Text(ImGui::IsMouseHoveringRect(ImGui::GetWindowContentRegionMin(), ImGui::GetWindowContentRegionMax()) ? "hovering" : "not");

                // pinging
                /*if (ImGui::Checkbox("pinging", &(this->pinging)))
                {
                    if (this->pinging)
                        this->startPinging();
                    else
                        cv.notify_all();
                }*/

                ImGui::PushFont(font_subtitle);
                {
                    /*if (ImGui::MenuItem("PINGING", this->pinging ? "on" : "off")) {
                        this->pinging = !(this->pinging);

                        if (this->pinging)
                            this->startPinging();
                        else
                            cv.notify_all();
                    }
                    ImGui::SetItemTooltip("constantly query server status?");*/

                    // TODO wip
                    /*if (ImGui::MenuItem("THEME", this->theme == THEME::dark ? "dark" : "light", nullptr, true)) {
                        setTheme((this->theme == THEME::dark) ? THEME::light : THEME::dark);
                        this->theme = (this->theme == THEME::dark) ? THEME::light : THEME::dark;
                    }*/

                    // network settings
                    static auto frame = ImVec2(18, 18);
                    static auto offset = ImVec2(14, 8);
                    static auto offset2 = ImVec2(130, 0);
                    {
                        static auto text = "network settings";
                        //static auto offset2 = ImGui::CalcTextSize(text) * ImVec2(1, 0);
                        if (ImGui::MenuItem(text, ""))
                        {
                            system("start windowsdefender://network");
                        }
                        list->AddImage(_get_texture("icon_outside_window"), ImGui::GetItemRectMin() + offset + offset2, ImGui::GetItemRectMin() + offset + offset2 + frame, ImVec2(0, 0), ImVec2(1, 1), color_button);
                        ImGui::SetItemTooltip("windowsdefender://network");
                    }

                    // firewall rules
                    {
                        static auto text = "firewall rules";
                        //static auto offset2 = ImGui::CalcTextSize(text) * ImVec2(1, 0);
                        if (ImGui::MenuItem("firewall rules", ""))
                        {
                            system("start wf.msc");
                        }
                        list->AddImage(_get_texture("icon_outside_window"), ImGui::GetItemRectMin() + offset + offset2, ImGui::GetItemRectMin() + offset + offset2 + frame, ImVec2(0, 0), ImVec2(1, 1), color_button);
                        ImGui::SetItemTooltip("wf.msc");
                    }

                    if (ImGui::MenuItem("AUTO UPDATE", options.auto_update ? "on" : "off", nullptr, true))
                    {
                        options.auto_update = !options.auto_update;
                    }

                    // TODO wip
                    if (ImGui::MenuItem("SAVE FILE", "off", nullptr, false))
                    {
                    }
                    ImGui::SetItemTooltip("create a save file? you will\nhave the option to delete it\nif you uninstall.\n\ndropship needs this to keep\noptions");

                    /*if (ImGui::Button("ADVANCED"))
                    {
                        // open popup
                    }*/

                }
                ImGui::PopFont();

                ImGui::PopItemFlag();
                ImGui::EndPopup();
            }
        }

        {
            if (ImGui::BeginPopupModal("socials", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
            {
                ImDrawList* list = ImGui::GetWindowDrawList();

                ImGui::PushItemFlag(ImGuiItemFlags_SelectableDontClosePopup, true);

                // handle close
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowHovered() && !ImGui::IsWindowAppearing())
                    ImGui::CloseCurrentPopup();


                ImGui::PushFont(font_title);
                ImGui::Text("OPTIONS");
                ImGui::PopFont();

                ImGui::PushFont(font_subtitle);
                {
                    static auto frame = ImVec2(18, 18);
                    static auto offset = ImVec2(180, 8);

                    // discord
                    if (ImGui::MenuItem("discord", "suggestions\nhelp", nullptr, true)) {
                        system("start https://discord.stormy.gg");
                    }
                    list->AddImage(_get_texture("icon_outside_window"), ImGui::GetItemRectMin() + offset, ImGui::GetItemRectMin() + offset + frame, ImVec2(0, 0), ImVec2(1, 1), color_button);

                    ImGui::Spacing();

                    // twitch
                    if (ImGui::MenuItem("twitch", "stormyy_ow", nullptr, true)) {
                        system("start https://twitch.tv/stormyy_ow");
                    }
                    list->AddImage(_get_texture("icon_outside_window"), ImGui::GetItemRectMin() + offset, ImGui::GetItemRectMin() + offset + frame, ImVec2(0, 0), ImVec2(1, 1), color_button);

                    // twitter
                    if (ImGui::MenuItem("twitter", "stormyy_ow", nullptr, true)) {
                        system("start https://twitter.com/stormyy_ow");
                    }
                    list->AddImage(_get_texture("icon_outside_window"), ImGui::GetItemRectMin() + offset, ImGui::GetItemRectMin() + offset + frame, ImVec2(0, 0), ImVec2(1, 1), color_button);

                    // github
                    if (ImGui::MenuItem("github", "code", nullptr, true)) {
                        system("start https://github.com/stowmyy");
                    }
                    list->AddImage(_get_texture("icon_outside_window"), ImGui::GetItemRectMin() + offset, ImGui::GetItemRectMin() + offset + frame, ImVec2(0, 0), ImVec2(1, 1), color_button);

                }
                ImGui::PopFont();


                ImGui::PopItemFlag();
                ImGui::EndPopup();
            }
        }

        //ImGui::End();
    }

    #ifdef _DEBUG
        {
            ImGui::Begin("debug");
            if (ImGui::CollapsingHeader("dashboard.c", ImGuiTreeNodeFlags_None))
            {
                
                {
                    if (ImGui::Button("new", { 60, 0 }))
                    {

                        //this->add_rule(NET_FW_PROFILE2_ALL);
                        firewallManager.AddFirewallRule(&(this->endpoints.at(0)), true);
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("flush", { 60, 0 }))
                    {
                        firewallManager.flushRules(&(this->endpoints));
                    }

                    if (ImGui::Button("sync (apply all)", { 200, 0 }))
                    {

                        firewallManager.sync(&(this->endpoints));
                    }

                    ImGui::SameLine();

                    if (ImGui::Button("sync endpoints", { 200, 0 }))
                    {

                        firewallManager._syncEndpointsWithFirewall(&(this->endpoints));
                    }

                    if (ImGui::Button("sync firewall", { 200, 0 }))
                    {

                        firewallManager._syncFirewallWithEndpoints(&(this->endpoints));
                        firewallManager._syncFirewallWithEndpoints(&(this->endpoints));
                    }

                }
            }


            ImGui::End();
        }
    #endif
}
