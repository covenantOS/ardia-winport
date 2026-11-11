# Windows skeleton engine report

## Wiring

`engine/patches/series.win` contains 66 Chromium patches in the original order, followed by:

```text
layer engine/chromium-win
layer engine/ardiacore-win
```

These replace `layer engine/chromium` and `layer engine/ardiacore`. Each layer's `apply.sh` delegates to its `apply-win.sh`, so a series runner's normal `apply.sh` convention works unchanged. The Windows driver must read `series.win` rather than the hard-coded `series` in the existing macOS `series.py`; inventory validation must use the files listed in `series.win`, not require every `.patch` in the directory.

`chromium-ardia-layer.patch` remains in the series and adds `//chrome/browser/ardia:exports` to `chrome_dll`. On Windows, `:exports` publicly depends on both `:ardia` and `//ardia/core`. `AC_ENGINE_EXPORT` is `__declspec(dllexport)` in the Windows public header.

The Windows apply scripts copy source only. They deliberately do not run the macOS script's Objective-C++ hooks or replace `ChromeMainDelegate` with `ACMainDelegate`.

## Series delta

Eight patches are omitted:

| Patch | Verification before omission |
|---|---|
| `chromium-devtools-redock-display` | All three targets are `app_shim_remote_cocoa` or `render_widget_host_view_mac.mm`. |
| `chromium-safe-storage-name` | Its only target is `components/os_crypt/common/keychain_password_mac.mm`. |
| `chromium-safe-storage-no-silent-reset` | All eight targets are Apple keychain or Mac OS Crypt implementations/tests. |
| `chromium-video-super-resolution` | Mixed; not simply omitted. Ten Mac-only targets are omitted and nine portable targets are retained in `.win`. |
| `chromium-keep-old-frame-on-swap` | All three targets are `BrowserCompositorMac` or `RenderWidgetHostViewMac`. |
| `chromium-angle-metal-shader-cache` | The target BUILD file is shared, but its only hunk restores the Metal `gpu_shader_cache.bin` framework resource; there is no portable hunk. |
| `chromium-background-instance-no-default-handlers` | Its only target is `chrome/browser/shell_integration_mac.mm`. |
| `chromium-apple-passwords-host-skipped` | `launch_context_posix.cc` is a shared path, but the only hunk skips `com.apple.passwordmanager`; there is no portable hunk. |

Four mixed patches use Windows variants:

| Variant | Retained | Removed |
|---|---|---|
| `chromium-video-super-resolution.win.patch` | `display.cc/.h`, `root_compositor_frame_sink_impl.cc/.h`, public `render_widget_host_view.h`, `display_private.mojom`, `compositor.cc/.h`, and `frame_data.h`. The behavioral additions are guarded by `BUILDFLAG(IS_MAC)` or `[EnableIf=is_mac]`. | All `BrowserCompositorMac`, `RenderWidgetHostViewMac`, `ImageTransportSurfaceOverlayMac`, `CALayerTreeCoordinator`, and `CARendererLayerTree` targets. |
| `chromium-cef-carryover.win.patch` | Browser lifetime, portable Views style, background-mode BUILD, and empty-tab-strip fixes. | Mac keyboard conversion, both `remote_cocoa` native-window implementations and their header, plus the ResourceBundle delegate API and caller dependency. |
| `chromium-window-hosted.win.patch` | Empty-Browser lifetime hooks in `browser.cc` and `unload_controller.cc`. | `bridged_content_view.h` and the hit-testing/accessibility implementation in `bridged_content_view.mm`. |
| `chromium-helium-branding.win.patch` | The other 36 target files. | `brave_full_disk_access_confirm_dialog_delegate_mac.mm`: `GetLinkText` and `OnLinkClicked` Safari-import help hunks. |

No target matching `remote_cocoa`, `app_shim_remote_cocoa`, `accelerated_widget_mac`, `crypto/apple`, `keychain_password_mac`, `shell_integration_mac`, or `*_mac.mm` remains in `series.win`.

## Symbol findings and stubs

The source audit did not support the premise that about 20 retained patches rely on globals defined only in `ardiacore_hooks.mm`. That file installs host callbacks but defines none of these globals. The retained patches define their own referenced globals and helper functions. `ac_seams.mm` and other `.mm` files primarily assign callbacks.

Consequently, no retained-patch identifier still needs a duplicate stub. `ac_stubs.cc` instead supplies 34 declarations needed by the copy-only Windows layer and future ports of the scripted hooks, without duplicating definitions already present in retained patches. All pointer bodies are `nullptr`, flags are `false` or zero, and functions are no-ops. Every signature was compared with the named declaration in `ac_hooks.h` or `ac_content_hooks.h`.

| Symbol | macOS definition/installer origin | Stub | Declaring header |
|---|---|---|---|
| `g_ardia_embedder` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `bool` | `ac_hooks.h` |
| `g_ardia_resource_bundle_delegate` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `ui::ResourceBundle::Delegate*` | `ac_hooks.h` |
| `g_ardia_viewless_browsers` | `apply.sh`; set in `ac_seams.mm` | `ac_stubs.cc`: `bool` | `ac_hooks.h` |
| `g_ardia_skip_cpu_health_sampling` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `bool` | `ac_hooks.h` |
| `g_ardia_background_instance` | background-instance patch; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `bool` | `ac_hooks.h` |
| `g_ardia_browser_window_factory` | `apply.sh`; installed by `ac_seams.mm` | `ac_stubs.cc`: `BrowserWindow* (*)(Browser*)` | `ac_hooks.h` |
| `g_ardia_create_web_contents_delegate` | `apply.sh`; installed by `ac_seams.mm` | `ac_stubs.cc`: eight-argument `std::unique_ptr<BrowserWebContentsDelegate>` pointer | `ac_hooks.h` |
| `g_ardia_create_permission_prompt` | `apply.sh`; installed by `ac_permissions.mm` | `ac_stubs.cc`: `(WebContents*, PermissionPrompt::Delegate*, bool*)` pointer | `ac_hooks.h` |
| `g_ardia_file_system_restore_prompt` | `apply.sh`; installed by `ac_permissions.mm` | `ac_stubs.cc`: `(const RequestData&, OnceCallback<PermissionAction>&, WebContents*)` pointer | `ac_hooks.h` |
| `g_ardia_external_protocol_dialog` | `apply.sh`; installed by `ac_external_apps.mm` | `ac_stubs.cc`: `(const GURL&, WebContents*, const optional<Origin>&, WeakDocumentPtr, const u16string&)` pointer | `ac_hooks.h` |
| `g_ardia_history_eligible` | `apply.sh`; installed by `ac_seams.mm` | `ac_stubs.cc`: `bool (*)(WebContents*)` | `ac_hooks.h` |
| `autofill::g_ardia_autofill_suggestions_shown` | `apply.sh`; installed by `ac_autofill_trigger.mm` | `ac_stubs.cc`: `(WebContents*, base::span<const Suggestion>)` pointer | `ac_hooks.h` |
| `g_ardia_open_action_popup` | `apply.sh`; installed by `ac_extension_view.mm` | `ac_stubs.cc`: `optional<bool> (*)(BrowserWindowInterface&, const Extension&, ShowPopupCallback&, string*)` | `ac_hooks.h` |
| `g_ardia_tab_shown_by_host` | `apply.sh`; installed by `ac_host_visibility.mm` | `ac_stubs.cc`: `int (*)(WebContents*)` | `ac_hooks.h` |
| `g_ardia_reserved_accelerator` | `apply.sh`; installed by `ac_seams.mm` | `ac_stubs.cc`: `bool (*)(const ui::Accelerator&)` | `ac_hooks.h` |
| `g_ardia_reserved_global_accelerator` | `apply.sh`; installed by `ac_seams.mm` | `ac_stubs.cc`: `bool (*)(const ui::Accelerator&)` | `ac_hooks.h` |
| `g_ardia_global_shortcuts_resumed` | `apply.sh`; installed by `ac_seams.mm` | `ac_stubs.cc`: `void (*)()` | `ac_hooks.h` |
| `g_ardia_safe_storage_key_may_be_created` | safe-storage patch; installed by `ac_safe_storage.mm` | `ac_stubs.cc`: `bool (*)()` | `ac_content_hooks.h` |
| `g_ardia_safe_storage_unreadable` | safe-storage patch; installed by `ac_safe_storage.mm` | `ac_stubs.cc`: `void (*)(int, base::OnceClosure)` | `ac_content_hooks.h` |
| `g_ardia_test_keychain` | safe-storage patch; installed by `ac_safe_storage.mm` | `ac_stubs.cc`: `crypto::apple::KeychainV2* (*)()` | `ac_content_hooks.h` |
| `g_ardia_permission_tab_shown` | `apply.sh`; installed by `ac_host_visibility.mm` | `ac_stubs.cc`: `int (*)(WebContents*)` | `ac_content_hooks.h` |
| `g_ardia_disk_space_check_seconds` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `int` | `ac_content_hooks.h` |
| `g_ardia_lazy_storage_usage` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `bool` | `ac_content_hooks.h` |
| `g_ardia_script_loader_ready_at_once` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `bool` | `ac_content_hooks.h` |
| `g_ardia_scripts_wait_at_response` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `bool` | `ac_content_hooks.h` |
| `g_ardia_hybrid_scan_on_demand` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `bool` | `ac_content_hooks.h` |
| `g_ardia_bluetooth_poll_on_demand` | `apply.sh`; set in `ac_main_delegate.cc` | `ac_stubs.cc`: `bool` | `ac_content_hooks.h` |
| `g_ardia_dnr_rule_matched` | `apply.sh`; installed by `ac_tab_bridge_navigation.mm` | `ac_stubs.cc`: declared four-argument `void` pointer | `ac_content_hooks.h` |
| `g_ardia_dnr_rulesets_in_force` | `apply.sh`; installed by `ac_navigation_hold.mm` | `ac_stubs.cc`: declared three-argument `void` pointer | `ac_content_hooks.h` |
| `download_crx_util::g_ardia_prompts_without_tab` | `apply.sh`; set in `ac_installed_bubble.mm` | `ac_stubs.cc`: `bool` | `ac_hooks.h` |
| `content::g_ardia_opacity_from_default_color` | `apply.sh`; set in `ac_tab_bridge.mm` | `ac_stubs.cc`: `bool` | `ac_content_hooks.h` |
| `ArdiaAutoPictureInPictureHostVisibilityChanged` | `apply.sh`; called by `ac_host_visibility.mm` | `ac_stubs.cc`: `void (WebContents*)` | `ac_hooks.h` |
| `ArdiaSetHybridUiShown` | `apply.sh` | `ac_stubs.cc`: `void (const void*, bool)` | `ac_content_hooks.h` |
| `extensions::ArdiaReregisterGlobalCommands` | `apply.sh`; called by `ac_extension_shortcuts.mm` | `ac_stubs.cc`: `void (content::BrowserContext*)` | `ac_hooks.h` |

`ac_hooks.h`, `ac_content_hooks.h`, and `ac_launch_switches.h` are verbatim copies. `ac_launch_switches.cc` has the same API without Security.framework; `InstanceValue` converts Windows' UTF-16 native switch value to UTF-8 and `SignedForDistribution` returns false.

## Kept-patch identifier resolution

The audit scanned whole identifiers in added code lines, with comments and string contents excluded. It accepted definitions in retained patches, the portable Windows layers/headers, or the new stubs.

| Resolution | Identifiers |
|---|---|
| 35 defined by retained patches | The 28 referenced `g_ardia_*` globals, plus `ArdiaDevToolsAgentHostForId`, `ArdiaFrameHasLiveConnection`, `ArdiaHasAccessedInitialMainDocument`, `ArdiaMainFrameHandlesEnterPictureInPicture`, `ArdiaMayInject`, `ArdiaPageHasLiveConnection`, and `ArdiaRestrictedSession`. |
| 3 supplied by portable layer/header files | `ArdiaDevToolsTokenClient`, `ArdiaPlaybackButton`, and `ArdiaVideoOverlayWindow`. |
| Unresolved | `[]` |

Total: 38 whole identifiers, zero unresolved. There were no whole-token `ac_*()` calls in the retained added code.

## Patch targets and behavior that cannot be stubbed

After the four `.win` substitutions, the path audit found no remaining Mac-only target hunks. The eliminated blockers are enumerated in the series-delta table above. A real helium-windows checkout was not present locally, so this verifies target paths and patch syntax, not context application or a Windows compile/link.

Two `*-without-browser-view` patches remain because their targets are portable:

- `chromium-storage-pressure-bubble-without-browser-view.patch`: `StoragePressureBubbleView::ShowBubble` must return before dereferencing a missing `BrowserView`.
- `chromium-remoting-dialog-without-browser-view.patch`: `MediaRemotingDialogCoordinatorViews::Show` must answer the permission callback with false before using a missing `BrowserView`.

Those are call-site behavior changes, not missing embedder symbols; a no-op ArdiaCore stub cannot reproduce them. The same applies to `chromium-window-hosted`'s NSView hit-testing and accessibility bridge. Its two `remote_cocoa` hunks are therefore omitted from the Windows series, while its portable Browser lifetime behavior remains.

## Verification performed

- Confirmed 66 patch entries and two Windows layer entries with the exact expected order and replacement map.
- Parsed all four new patches with `git apply --stat`.
- Scanned all retained targets for Mac-only paths; no matches.
- Scanned both Windows layers for Objective-C++ files and Cocoa/AppKit/Foundation/Security/other Apple imports; no matches.
- Verified all three copied ArdiaCore headers byte-for-byte.
- Verified all 34 stub symbols have declarations in the copied hook headers.
- Ran `apply-win.sh`, `apply-win.sh --check`, and `apply.sh --check` successfully for both layers against separate scratch Chromium-shaped trees.
- Ran the kept-patch whole-identifier resolution audit; unresolved list was empty.
- Ran `bash -n` on all four apply scripts.
