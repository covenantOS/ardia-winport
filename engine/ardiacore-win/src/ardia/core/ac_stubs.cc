#include "ardia/core/ac_hooks.h"

bool g_ardia_embedder = false;
ui::ResourceBundle::Delegate* g_ardia_resource_bundle_delegate = nullptr;
bool g_ardia_viewless_browsers = false;
bool g_ardia_skip_cpu_health_sampling = false;
bool g_ardia_background_instance = false;
BrowserWindow* (*g_ardia_browser_window_factory)(Browser*) = nullptr;

std::unique_ptr<BrowserWebContentsDelegate> (
    *g_ardia_create_web_contents_delegate)(
    BrowserWindowInterface* browser,
    ExclusiveAccessManager& exclusive_access_manager,
    chrome::BrowserCommandController& command_controller,
    UnloadController& unload_controller,
    web_app::AppBrowserController* app_browser_controller,
    BrowserWindow& window,
    DesktopBrowserWindowCapabilities& capabilities,
    BrowserUiController& browser_ui_controller) = nullptr;

std::unique_ptr<permissions::PermissionPrompt> (
    *g_ardia_create_permission_prompt)(
    content::WebContents* web_contents,
    permissions::PermissionPrompt::Delegate* delegate,
    bool* default_handling) = nullptr;

bool (*g_ardia_file_system_restore_prompt)(
    const FileSystemAccessPermissionRequestManager::RequestData& request,
    base::OnceCallback<void(permissions::PermissionAction)>& callback,
    content::WebContents* web_contents) = nullptr;

bool (*g_ardia_external_protocol_dialog)(
    const GURL& url,
    content::WebContents* web_contents,
    const std::optional<url::Origin>& initiating_origin,
    content::WeakDocumentPtr initiator_document,
    const std::u16string& program_name) = nullptr;

bool (*g_ardia_history_eligible)(content::WebContents*) = nullptr;
std::optional<bool> (*g_ardia_open_action_popup)(
    BrowserWindowInterface& browser,
    const extensions::Extension& extension,
    ShowPopupCallback& callback,
    std::string* error) = nullptr;
int (*g_ardia_tab_shown_by_host)(content::WebContents*) = nullptr;
bool (*g_ardia_reserved_accelerator)(const ui::Accelerator& accelerator) =
    nullptr;
bool (*g_ardia_reserved_global_accelerator)(
    const ui::Accelerator& accelerator) = nullptr;
void (*g_ardia_global_shortcuts_resumed)() = nullptr;
bool (*g_ardia_safe_storage_key_may_be_created)() = nullptr;
void (*g_ardia_safe_storage_unreadable)(int status,
                                       base::OnceClosure retry) = nullptr;
crypto::apple::KeychainV2* (*g_ardia_test_keychain)() = nullptr;
int (*g_ardia_permission_tab_shown)(content::WebContents*) = nullptr;
int g_ardia_disk_space_check_seconds = 0;
bool g_ardia_lazy_storage_usage = false;
bool g_ardia_script_loader_ready_at_once = false;
bool g_ardia_scripts_wait_at_response = false;
bool g_ardia_hybrid_scan_on_demand = false;
bool g_ardia_bluetooth_poll_on_demand = false;

void (*g_ardia_dnr_rule_matched)(
    content::BrowserContext* browser_context,
    int tab_id,
    const extensions::declarative_net_request::RequestAction& action,
    const extensions::WebRequestInfo& request) = nullptr;

void (*g_ardia_dnr_rulesets_in_force)(
    content::BrowserContext* browser_context,
    const extensions::ExtensionId& extension_id,
    bool all_loaded) = nullptr;

void ArdiaAutoPictureInPictureHostVisibilityChanged(
    content::WebContents* contents) {}

void ArdiaSetHybridUiShown(const void* model, bool shown) {}

namespace autofill {

void (*g_ardia_autofill_suggestions_shown)(
    content::WebContents* web_contents,
    base::span<const Suggestion> suggestions) = nullptr;

}

namespace download_crx_util {

bool g_ardia_prompts_without_tab = false;

}

namespace content {

bool g_ardia_opacity_from_default_color = false;

}

namespace extensions {

void ArdiaReregisterGlobalCommands(content::BrowserContext* context) {}

}
