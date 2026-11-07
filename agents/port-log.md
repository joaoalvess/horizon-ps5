# Port log

Stage 3 of [PLAN.md](../PLAN.md): the `port` units, and the `depends` units whose prerequisites come too, cherry-picked onto the core in upstream date order. Writer: the porting agent. The work clone is `/Volumes/Hangar/ps5/core-v2`, branch `horizon-v2` (the pin plus the 17 EglantinePS5 patches as commits). Its commits came into `v2` with `git format-patch -k | git am -k --directory=Eglantine`, keeping authors, dates and messages; `HEAD:Eglantine` has the same tree hash as `horizon-v2`. The hashes in Notes are `horizon-v2`'s: in `v2` the same commit is found by its upstream hash, `git log --grep 'cherry picked from commit <hash>'`, or by its subject.

Status: `picked` (applied as upstream wrote it), `adapted` (changed to fit; Notes say how), `partial` (only what the Why names), `deferred` (not ported tonight; Notes say why), `dropped` (a `depends` whose prerequisite does not come).

| Date | Unit | File | Verdict | Commits | Diff | Status | Notes |
|---|---|---|---|---|---|---|---|
| 2026-10-30 | dj:c96d1e0193 | loader-fs | port | 1 | `24e3cb462e..c96d1e0193` | picked | 54f09b3ed4. |
| 2026-10-30 | dj:f669bbbfff | loader-fs | port | 1 | `c96d1e0193..f669bbbfff` | picked | 938685673f. |
| 2026-10-30 | dj:affa8969ac | gpu-core | port | 1 | `ca524c9aa8..affa8969ac` | picked | 0a79c8a309, include conflict in fence_manager.h resolved. |
| 2026-10-30 | dj:1b7c637c61 | gpu-core | port | 1 | `b40ad217ab..1b7c637c61` | picked | e1eb382e35, kept patch 0004's LoadingProbe marks beside CoveredRows. |
| 2026-10-30 | dj:be76402ab5 | gpu-vulkan | port | 1 | `b40ad217ab..be76402ab5` | adapted | 478dfe37a9, the disk-cache check moved into SerializeGraphicsPipeline, which both of the port's graphics create paths call; anchors and cmake rewrites still match. |
| 2026-10-31 | dj:b096fbea66 | gpu-core | port | 1 | `1b7c637c61..b096fbea66` | picked | 427990e396, the LoadingProbe writeback marks moved inside the new tmp_buffer2 scope. |
| 2026-10-31 | dj:1b726d93c5 | services | port | 1 | `b40ad217ab..1b726d93c5` | picked | f2d8e9d872, comment conflict in nx2_gsv.h resolved. |
| 2026-10-31 | dj:28b23be30f | services | port | 1 | `b40ad217ab..28b23be30f` | picked | 691de68037. |
| 2026-10-31 | dj:28f66d0c8b | services | port | 1 | `b40ad217ab..28f66d0c8b` | picked | 6925584d22. |
| 2026-10-31 | dj:520826452f | services | port | 1 | `b40ad217ab..520826452f` | picked | 029b4c3f7c. |
| 2026-10-31 | dj:914423e87d | cpu-jit | port | 1 | `2b72e34856..914423e87d` | picked | 02ae2628d4, the A64 hunk was already in the pin (only its comment added). |
| 2026-10-31 | dj:140489e51f | services | port | 1 | `8ac95d5e42..140489e51f` | picked | ee34431d8f. |
| 2026-10-31 | dj:574517814d | services | port | 1 | `8ac95d5e42..574517814d` | picked | 3f71e2029f. |
| 2026-10-31 | dj:049c9633ed | gpu-core | port | 1 | `8ac95d5e42..049c9633ed` | picked | a7659c235f, with dj#337's tip form e46c6cbfb0 (9b087b8b8b) gating the header offset on Switch 2. |
| 2026-10-31 | dj:77ea16e2f2 | shaders | port | 1 | `9dca3ab2e4..77ea16e2f2` | picked | 70f9cb9db5. |
| 2026-10-31 | dj:561f87abec | shaders | port | 1 | `77ea16e2f2..561f87abec` | picked | de2d67a102; its dj#337 follow-ups come with dj:e23b0f2fab. |
| 2026-10-31 | dj:75385e994d | gpu-core | port | 1 | `9dca3ab2e4..75385e994d` | picked | 752fbe6336, member conflict in texture_cache_base.h resolved; anchors match. |
| 2026-10-31 | dj:8266a6a91b | gpu-core | port | 1 | `9dca3ab2e4..8266a6a91b` | adapted | eb2d117477, the mode table replaces patch 0007's one-partition fast path and its EDEN_NX2_ASTC_OPAQUE_SLOW switch. |
| 2026-10-31 | dj:dbd9572a5a | gpu-core | port | 1 | `e36d46693b..dbd9572a5a` | adapted | ba470b3161, lazy clear in descriptor_table.h only; texture_cache.h keeps patch 0007's EDEN_NX2_TC_CORRUPT_IDS gate. |
| 2026-10-31 | dj:5de94bdd1d | gpu-vulkan | depends | 1 | `be76402ab5..5de94bdd1d` | picked | 9da0b69127. |
| 2026-11-01 | dj:edabc3e614 | gpu-core | port | 1 | `37908985af..edabc3e614` | picked | 01e02768a0, LoadingProbe marks kept around the memcpy. |
| 2026-11-01 | dj:8551ee6462 | gpu-vulkan | port | 1 | `adae763570..8551ee6462` | partial | 48ef90cf57, compute reset only, as the Why says; graphics hunk left out. |
| 2026-11-01 | dj:446c34aa8b | gpu-core | depends | 1 | `75385e994d..446c34aa8b` | picked | 8674bbec5c. |
| 2026-11-01 | dj:3871de9da9 | gpu-core | port | 1 | `adae763570..3871de9da9` | picked | c6be344655. |
| 2026-11-01 | dj:594e07ee53 | gpu-core | port | 1 | `adae763570..594e07ee53` | picked | 70d04188ec; prepare-vulkan-port.py's collector anchor moved to the new 512 MiB / 1 GiB thresholds, memory_short kept. |
| 2026-11-01 | dj:ab943ab4f3 | gpu-core | depends | 1 | `b096fbea66..ab943ab4f3` | picked | d8cffbedeb. |
| 2026-11-01 | dj:9039dfe963 | services | port | 1 | `b40ad217ab..9039dfe963` | picked | 409be03d2b. |
| 2026-11-01 | dj:bbe3464f41 | services | port | 1 | `b40ad217ab..bbe3464f41` | picked | 74a3bb21bb. |
| 2026-11-01 | dj:fa1f6b1d7b | shaders | port | 1 | `6a10b27507..fa1f6b1d7b` | picked | 96a0fbf4df, the pin's geometry-stage gate on 17/29 kept. |
| 2026-11-01 | dj:9df12f0051 | shaders | port | 1 | `f7253f24f7..9df12f0051` | adapted | 0ee1e1836e, kept the pin's F16 fetch; .LL refused on the descriptor-conflict path and for F16 arrays. Then 9d6f094576: 0xb66's path refuses bit 91, the uniform-handle flag 0xf66 requires, which it ignored and .LL forms now reach. |
| 2026-11-01 | dj:f9c6ae332a | shaders | port | 1 | `9df12f0051..f9c6ae332a` | picked | 537551507f, on the pin's required_surface_buffer. |
| 2026-11-01 | dj:2f2b0a7468 | shaders | port | 1 | `f9c6ae332a..2f2b0a7468` | picked | dc6445e095. |
| 2026-11-01 | dj:d88554cedb | shaders | port | 1 | `2f2b0a7468..d88554cedb` | picked | c52ebfafd7, upstream's operation check replaces the pin's. |
| 2026-11-01 | dj:6e38b4c8fb | shaders | port | 1 | `d88554cedb..6e38b4c8fb` | adapted | 139c3ae5b3, XOR merged into the pin's REDUX with MIN (its OR flag became the OR/XOR selector). |
| 2026-11-01 | dj:b52f29993f | shaders | port | 1 | `572c89d713..b52f29993f` | adapted | 29ab5229ce, the pin's own case 0x80d that refused CSMTEST removed and ampere_kirby.cpp's clip-state test turned into a CSMTEST lowering check. |
| 2026-11-01 | dj:6a96ec2cdc | shaders | port | 1 | `fdca67d731..6a96ec2cdc` | picked | 87e4bbde75. |
| 2026-11-01 | dj:e71bb09e2d | shaders | port | 1 | `6a96ec2cdc..e71bb09e2d` | picked | bf1f5d454b, 0x896 added to the pin's uniform lists and guard path. |
| 2026-11-01 | dj:7998b0bc55 | shaders | port | 1 | `151ad5c598..7998b0bc55` | picked | 5983129a89, 0x8a5 added to the pin's uniform lists. |
| 2026-11-01 | dj:f963645e89 | shaders | port | 1 | `7998b0bc55..f963645e89` | picked | 7e95be430f. |
| 2026-11-01 | dj:f4d79570e9 | shaders | port | 1 | `f74256da81..f4d79570e9` | picked | cac502116c, on the pin's 64-bit-aware source limit. |
| 2026-11-01 | dj:7aa9857432 | shaders | port | 1 | `23b194b84a..7aa9857432` | adapted | ecb5204e8c, TXQ.B joins the TXQ case with the pin's own TXQ.B form tried first. |
| 2026-11-01 | dj:b14a268ebc | shaders | port | 1 | `7aa9857432..b14a268ebc` | picked | b1afa75ce5. |
| 2026-11-01 | dj:6e289d8935 | shaders | port | 1 | `b14a268ebc..6e289d8935` | picked | cf5675ff09. |
| 2026-11-01 | dj:563ed8e60b | shaders | port | 1 | `6e289d8935..563ed8e60b` | adapted | 58f3834774, uniform-handle form merged into the pin's TLD4 with per-tap offsets (refused for 0xf63). |
| 2026-11-01 | dj:a08454340e | shaders | port | 1 | `563ed8e60b..a08454340e` | adapted | 77e765aeeb, the pin's general_tex already takes TEX.B with a LOD; only the five-or-more-value split is taken into it. |
| 2026-11-01 | dj:e398258071 | shaders | depends | 1 | `a08454340e..e398258071` | picked | 5e1b11d9e3. |
| 2026-11-01 | dj:01389ee526 | shaders | port | 1 | `e398258071..01389ee526` | adapted | 10d1393ee9, LowerToMaxwellWith keeps the pin's geometry and cooperative-matrix arguments; the pin's 216 limits follow high_uniform_base. Then d7d8da1458: the uniform bindless destination check captures high_uniform_base. |
| 2026-11-01 | dj:cefcf3a9b1 | shaders | port | 1 | `01389ee526..cefcf3a9b1` | adapted | 477dcd0563, look-back merged with the pin's any-UPn clamp match. |
| 2026-11-01 | dj:27cf4285e0 | shaders | depends | 1 | `b0043b6065..27cf4285e0` | adapted | 9730da0f66, AOFFI taken as the pin's offset mode 1 beside its per-tap mode 2. |
| 2026-11-01 | dj:6a95f4fb29 | shaders | depends | 1 | `27cf4285e0..6a95f4fb29` | adapted | e7e7909e1c, F16 bit 79 stays open; offsets refused on the pin's descriptor-conflict path. |
| 2026-11-01 | dj:6d9821d50b | gpu-vulkan | port | 1 | `02504aeb37..6d9821d50b` | picked | 98db590450; anchors match. |
| 2026-11-01 | dj:a999d34f39 | gpu-vulkan | depends | 1 | `2f94fb9680..a999d34f39` | picked | ec354024cf; anchors match. |
| 2026-11-01 | dj:22c50e7cab | loader-fs | port | 1 | `6bde8c20d1..22c50e7cab` | picked | 2725008a0c; port GetFiles rewrite in headless/CMakeLists.txt drops the parent path argument. |
| 2026-11-01 | dj:eca63ccb10 | loader-fs | port | 1 | `6bde8c20d1..eca63ccb10` | picked | 23449393a1; tools/check-vfs-file-budget.py already takes the new DropReference. |
| 2026-11-01 | dj:f398ff2e17 | gpu-core | port | 1 | `6bde8c20d1..f398ff2e17` | picked | d99bb0dc5a. |
| 2026-11-01 | dj:dd8742e563 | services | port | 1 | `6bde8c20d1..dd8742e563` | picked | 2f53622ea1; upstream's 0x1F<<40 mask taken. |
| 2026-11-01 | dj:20df0fac21 | services | port | 1 | `6bde8c20d1..20df0fac21` | adapted | 0a9342f555, 15/16 use upstream's single notification event; 17/18 keep the pin's per-id output notifications. |
| 2026-11-01 | dj:efe9283de5 | services | port | 1 | `20df0fac21..efe9283de5` | adapted | 75ac823512, only 220 taken; the pin already serves 213 as SendNx2VibrationValue. |
| 2026-11-01 | dj:b878201f33 | services | port | 1 | `d5c8b1cbed..b878201f33` | adapted | 31406bce2b, kept SucceedQuietly for the pin's 13, 51, 111, 112, 251, 252. |
| 2026-11-01 | dj:d5c8b1cbed | services | port | 1 | `72a8b41fd5..d5c8b1cbed` | adapted | edf9325940, the pin's vi2 11/13/21 kept; 31/32 become per-slot native events, the pin's GetVsyncEvent removed. |
| 2026-11-01 | dj:7f31fc4f53 | services | port | 1 | `5b4695e5f2..7f31fc4f53` | picked | 3cd68a259f; gpu Handshake placed beside the pin's GpuCommands alias. |
| 2026-11-01 | dj:c26cfaeac4 | services | depends | 1 | `7f31fc4f53..c26cfaeac4` | adapted | a1707367af, the pin's report-once repeat path also replies zeroed. |
| 2026-11-01 | dj:a07b8a3190 | services | depends | 1 | `c26cfaeac4..a07b8a3190` | adapted | 8bd260425e, pin's report-once stub, bsd 36 and am 2400 kept; stub rows shadowed by pin handlers left out. |
| 2026-11-01 | dj:37446d60a6 | services | depends | 1 | `a07b8a3190..37446d60a6` | adapted | 13ca7d128f, 115 stays the pin's ConfigureNativeMemory (0 keeps the nvdrv path); QueryGrantedFeatures left out. |
| 2026-11-01 | dj:2f803fa227 | services | depends | 1 | `37446d60a6..2f803fa227` | picked | f1d8582345; the pin's image handlers serve the feature groups. |
| 2026-11-01 | dj:c5a270afc8 | services | depends | 1 | `2f803fa227..c5a270afc8` | picked | 5144465db0. |
| 2026-11-01 | dj:e23b0f2fab | shaders | depends | 1 | `561f87abec..e23b0f2fab` | picked | 9ba3a73b20. |
| 2026-11-02 | dj:cce6d44af2 | services | port | 1 | `803ece5f90..cce6d44af2` | picked | 7d4e9fcb41 with dj#337's 256b1dc346 as 6d449d5ab7. |
| 2026-11-02 | dj:6612817cb4 | gpu-core | depends | 1 | `51d798ff03..6612817cb4` | adapted | e95edc7a24, the pin's FlushGpuWrittenCommands drain gate kept; IsGpuWrittenCommands added beside it. |
| 2026-11-02 | dj#270 | shaders | port | 23 | `b8d2c08d6e..9c55e70d79` | partial | 9604eaee2a..d4af5fba09; 43ab1ee42c adapted to 0xc0a, IDP.2A, FMNMX.NAN; out: 9cf6a499c8 (104), 681d50cd9c (empty), 18b0671e1b, 2b8002474d, 93f8c89595, 44c0fa86e2, ef3ae4ae23 (in pin), 68f8d96368, 9f22054c63 (pin redesign), d125cfb41c, 92d49d8a0c (FE DLSS); no merge residual. |
| 2026-11-02 | dj#264 | gpu-core | port | 1 | `926e7ac6af..46b2577bf7` | picked | b53deab5f5; anchors match. |
| 2026-11-02 | dj#260 | common | port | 1 | `6184688806..a407703f6d` | partial | 5deb7b97ca, only the buffer_cache.h page bound; fiber, host_memory, ASTC flush in port; payload stays the PS5 vector; GC eviction and transcode wait left for console A/B. |
| 2026-11-02 | dj:cd9daa8d00 | shaders | port | 1 | `9210b8d69d..cd9daa8d00` | dropped | empty, fae48f1f7b (dj:5dde7e9040) already lowers I2F.S8 and S16 in the pin form. |
| 2026-11-02 | dj:855559a68e | shaders | port | 1 | `75bdb925df..855559a68e` | adapted | 7e76e6d737, only the G+B and R+B TLD.SCR form (guarded USHF came with dj:201aacdae5, LEA.HI constant is the pin). |
| 2026-11-02 | dj#318 | frontend-build | port | 3 | `75bdb925df..ead60c761e` | adapted | df5587075b, only 8185d45d5d; its nup_writer test hunk left out with the NUP writer. |
| 2026-11-02 | li:d1b7f69df7 | frontend-build | port | 1 | `5bef7354b4..d1b7f69df7` | partial | 9a54bfb257, only pipeline_cache_files.h and vk_pipeline_cache.cpp; tools/check-pipeline-cache-console.py still names vulkan_pipelines.bin. |
| 2026-11-02 | dj#322 | gpu-core | depends | 1 | `ead60c761e..62214094d6` | deferred | measure against the pin's list drain (patches 0006/0007, EDEN_NX2_LIST_DRAIN) on the console. |
| 2026-11-02 | dj:9598eadeb4 | shaders | port | 1 | `855559a68e..9598eadeb4` | adapted | 1ec6fbea55, only the U8/S8/U16/S16 LDL/STL in the pin lowering (guarded UIADD3 is the pin). |
| 2026-11-02 | dj:85ec271c1a | shaders | port | 1 | `62214094d6..85ec271c1a` | dropped | empty, the same SHF.L.U32.HI change came with dj:201aacdae5 (01aa90740c). |
| 2026-11-02 | li:b3f983a6b8 | shaders | depends | 1 | `8cd5c6a44b..b3f983a6b8` | dropped | needs li:8cd5c6a44b and dj#465 f130f0c50a, not in the log. |
| 2026-11-02 | li:fae3bc64a9 | gpu-core | depends | 1 | `de6efea6d3..fae3bc64a9` | dropped | not needed: the pin flushes macro parameters only on GPU-written pages, which IsGpuWrittenCommands (dj:6612817cb4) already keeps off the snapshot; its command_fetch_order.h is lichii's. |
| 2026-11-03 | dj#337 | loader-fs | depends | 19 | `05259375e0..155bf34fc2` | partial | only 9f4f01f76e (b3902191a3, MapInOrder redone on Nx2GpuCommandScope, braces fixed by carry aba82d7afd) for Pokopia's first frame with li#110's #543; the rest needs dj:0f14696044, dj#281, dj:130c255ed9, which do not come; follow-ups of picked units taken with them: c83eb1d1f5, bda49e810b, 8b0666b748, 0ed4a3a9e7 (d8a71a0081..7ca1fe556a), e46c6cbfb0, 256b1dc346, 8556e27015 (ff708807b4). |
| 2026-11-02 | dj:92a1a0f703 | gpu-vulkan | port | 1 | `8362c44ff8..92a1a0f703` | picked | 3b6028bab1; vk_rasterizer.cpp anchors match. |
| 2026-11-02 | dj:e4422115d6 | shaders | port | 1 | `9598eadeb4..e4422115d6` | adapted | b2f2e23f30, F2I absolute source in the pin F2I and MOV from any UR; DEPBAR and constant LEA kept from dj:201aacdae5. |
| 2026-11-02 | dj:66d05284d9 | shaders | port | 1 | `8362c44ff8..66d05284d9` | adapted | 82e5cd77ab, with dj:512536d916 0x988 hunks only as 3778c855f6. |
| 2026-11-02 | dj:e2b4ab1b6f | shaders | port | 1 | `66d05284d9..e2b4ab1b6f` | picked | f1fdc595d5. |
| 2026-11-03 | dj#304 | shaders | port | 3 | `0f9760e5cc..efa7c19e59` | adapted | e13513f658, 923e02845e, 65a54c568c; F16 pack on TLD4 only (the pin's TLDS keeps TLD.SCR); merge resolution carried in 9b619a8566. |
| 2026-11-02 | dj#296 | input | port | 1 | `c0cb3325e2..8c63450666` | dropped | same patch as dj#495. |
| 2026-11-03 | dj#356 | gpu-core | port | 6 | `2d81fd5072..aae778edce` | partial | e9b074a433, 25006240f2, d6b4d216f0, fed3d26801; 8d6cb5c0d4 empty (in pin), 701c91b402 left out (the pin detects H.264 from the configuration's SPS/PPS). |
| 2026-11-02 | dj#313 | services | port | 13 | `bd12e7c4ef..28051fc764` | deferred | the port re-implements lan_discovery.cpp, socket_proxy.cpp, network.cpp and bsd.cpp in multiplayer_packets.cmake; redoing those rewrites for its 13 commits is past the time box (multiplayer only). |
| 2026-11-03 | dj#155 | services | port | 2 | `e5e82c1bb0..68a945d0a8` | adapted | 815479a5cc, 3293c467df; the pin's 213 SendNx2VibrationValue kept, 214 taken. |
| 2026-11-03 | dj:15126fbdc1 | gpu-core | port | 1 | `68a945d0a8..15126fbdc1` | picked | 598ab34b3d; buffer_cache.h anchors match. |
| 2026-11-03 | dj#349 | kernel-memory | port | 1 | `7fbaea9585..f201586db8` | adapted | c62231ea95, NSP branch only, without dj#258's NUP branch. |
| 2026-11-03 | dj#361 | gpu-vulkan | port | 1 | `ad7282cd73..f50099038e` | picked | 6d8cf18cb8; anchors match. |
| 2026-11-03 | dj#374 | gpu-core | port | 1 | `881a477514..a667551cdf` | picked | d16ac4da4b; anchors match. |
| 2026-11-03 | dj#464 | shaders | port | 2 | `79c4f8215a..7adcecc6d2` | adapted | 433829b23e, 86bcfe07f1; CACHE_VERSION GL 22, VK 26; headless/shader_cache_reader.inc checks six leading sizes (headless/shader_cache_check.cpp still writes five). |
| 2026-11-04 | dj:2ff03ff362 | gpu-core | depends | 3 | `541a9ee0fa..2ff03ff362` | picked | fac885afd7, 1bc1a66329, f6975b5dd6 (dj:1b7c637c61 is picked). |
| 2026-11-04 | dj:5ae14067e3 | loader-fs | depends | 3 | `93b25f9a6b..5ae14067e3` | dropped | needs dj#334, which is not in the port. |
| 2026-11-04 | dj:5dde7e9040 | shaders | port | 3 | `8604d2561a..5dde7e9040` | partial | fae48f1f7b, 7ee9cc3f3b; b12e25f166 left out (needs dj#322); kept the pin post-exit call rule. Then c5c20164c5: its G8R24 case dropped, the pin already maps it. |
| 2026-11-04 | dj:8604d2561a | gpu-core | depends | 1 | `a6341c51d1..8604d2561a` | dropped | the Annex-B conversion it replaces is not in the pin (dj#356's 701c91b402 left out; the pin parses the H.264 configuration itself). |
| 2026-11-04 | dj:93b25f9a6b | gpu-core | port | 1 | `c8ecbd40c8..93b25f9a6b` | picked | 1012cdaace; maxwell_3d.cpp rewrites still match. |
| 2026-11-04 | dj:c8ecbd40c8 | gpu-core | port | 1 | `2ff03ff362..c8ecbd40c8` | picked | 1c164eebfc; maxwell_3d.cpp rewrites still match. |
| 2026-11-04 | dj:0e8c727c26 | cpu-jit | port | 4 | `1dceba16cc..0e8c727c26` | picked | 3e1a25fbdf, cca16a62bc, 97495a568f, a8d9fd2076; the a64_translate.cpp copy gains one include of a new impl file. |
| 2026-11-04 | dj:201aacdae5 | shaders | port | 1 | `960a6a9ca4..201aacdae5` | adapted | 01aa90740c without FFMA_reg and the LEA.HI no-.X branch (the pin 0xa11 lowers it, test expects its two adds); merge residual is only FFMA_reg and guards the pin already has. |
| 2026-11-04 | dj:47882d142b | gpu-core | depends | 1 | `581dec4201..47882d142b` | dropped | its Why needs dj:dbd9572a5a, dj#334, dj:3871de9da9, none in the port. |
| 2026-11-04 | dj:699903a926 | gpu-vulkan | port | 1 | `917a3344c2..699903a926` | picked | a879ebcfbc; configure_phases anchors match. |
| 2026-11-04 | dj:a752ea5cec | kernel-memory | port | 1 | `0e8c727c26..a752ea5cec` | adapted | a824ffa88b; GetContiguousSpan beside the pin TryPublishedSemaphoreRead; merge leaves no residual. |
| 2026-11-04 | dj:c7c97429e1 | services | port | 1 | `3d9a618c7c..c7c97429e1` | adapted | 88f6eb48f3; retires the old backing through the pin ReleaseBacking callback as 215 does, bytes counted once in FreeReleasedRegions. |
| 2026-11-04 | dj:e16a9ce266 | services | port | 1 | `14c50942eb..e16a9ce266` | picked | 2f9889793a. |
| 2026-11-04 | dj:93821e84c7 | shaders | depends | 1 | `6e20fc2a6e..93821e84c7` | dropped | empty, dj:201aacdae5 was taken without the FFMA_reg case and operand form it removes. |
| 2026-11-03 | dj:313463cd27 | gpu-vulkan | depends | 1 | `660e5f0039..313463cd27` | dropped | needs dj:660e5f0039 (measure), not in the log. |
| 2026-11-03 | li:de8e2defdf | gpu-vulkan | port | 1 | `3483630ccb..de8e2defdf` | picked | bde1eb675c in place of dj:174b8039ec; anchors match. |
| 2026-11-04 | li:7a9dd37103 | gpu-core | depends | 1 | `7d9596a4be..7a9dd37103` | dropped | needs li:8cd5c6a44b, not in the log. |
| 2026-11-04 | li:ca6b8e3ca0 | frontend-build | depends | 1 | `ba2d291bae..ca6b8e3ca0` | dropped | needs dj#363 (measure) for UsesNvnStorageDescriptor, not in the port. |
| 2026-11-04 | li:b785035c5f | frontend-build | depends | 1 | `ca6b8e3ca0..b785035c5f` | adapted | b3cbe8840a without the DkbFrameTrace logging (dj#270 pacing and guest sleep are in). |
| 2026-11-04 | li:078b720a2f | gpu-core | depends | 1 | `7a9dd37103..078b720a2f` | dropped | needs li:7a9dd37103, dropped. |
| 2026-11-04 | li:4a58842d23 | gpu-core | depends | 1 | `9a435b584a..4a58842d23` | dropped | needs li:f6a8f51324 (skip). |
| 2026-11-04 | li:45e4cfb18d | services | depends | 1 | `9a435b584a..45e4cfb18d` | dropped | needs dj:8604d2561a, dropped. |
| 2026-11-04 | li:8ff071d330 | frontend-build | port | 1 | `07a155d7b5..8ff071d330` | picked | 8b1d996e93; patch 0004 DefineInputs anchors match. |
| 2026-11-03 | dj:9fea7d8379 | gpu-core | depends | 1 | `68a945d0a8..9fea7d8379` | adapted | a0a1e704b0 (dj:cce6d44af2 is picked); keeps the pin acquire-resume capture. |
| 2026-11-04 | dj#237 | gpu-vulkan | port | 1 | `36251b2191..0d86405bd1` | picked | e3bb3cb7fc; vk_rasterizer.cpp anchors match. |
| 2026-11-04 | dj:512536d916 | gpu-core | depends | 1 | `04727488a8..512536d916` | dropped | needs dj:b13c4c1c0c, not in the log; its two 0x988 hunks came with dj:66d05284d9 as 3778c855f6. |
| 2026-11-04 | dj#495 | services | port | 3 | `55cb3155ef..fc1c8fedff` | picked | f15704f53e, 0255c612dd, 74f4e8b771. |
| 2026-11-03 | dj:55274de6af | gpu-vulkan | depends | 1 | `512536d916..55274de6af` | dropped | needs dj:313463cd27, dropped. |
| 2026-11-04 | dj:179b5afb50 | gpu-core | depends | 1 | `8615d8d53e..179b5afb50` | dropped | needs dj#233 (measure), not in the log. |
| 2026-11-04 | li#47 | frontend-build | depends | 1 | `6dd32626e6..448fdaed95` | dropped | needs li:f6a8f51324 (skip) and li:ca6b8e3ca0, dropped. |
| 2026-11-04 | li#48 | services | depends | 1 | `add7922124..5c6153bc1d` | adapted | 7896500d35 (dj#356 partial carries the Finalize handler), then ff81e30ca4: its Start and Finalize trace lines use Nx2VideoTraceEnabled, m_decoded_frames and m_end_queued from li:45e4cfb18d, which is dropped, so they go (the build failed on them). |
| 2026-11-04 | dj#505 | services | port | 1 | `5cad158835..c54bc2289b` | picked | c05c728577. |
| 2026-11-04 | li#54 | gpu-core | depends | 1 | `f12b753f02..9479d8fa61` | dropped | needs dj#322, deferred. |
| 2026-11-04 | li#59 | gpu-core | depends | 1 | `d48f75bdfb..9960298c96` | dropped | needs dj:660e5f0039 (measure), not in the log. |
| 2026-11-04 | li#57 | frontend-build | depends | 1 | `9960298c96..cf1beed4be` | picked | 7acab9ad32 (dj:01389ee526 is adapted); additive beside the pin SURED case. |
| 2026-11-04 | li#58 | frontend-build | port | 1 | `cf1beed4be..325e0a867b` | adapted | 09f0b8144a, 0x499 in the pin uniform-bank and guard lists. Then f95684ab1d: its test calls LowerToMaxwellWith in dj:01389ee526's argument order. |
| 2026-11-04 | dj#512 | kernel-memory | depends | 1 | `c54bc2289b..39c42e645b` | dropped | its NUP branch comes from dj#258 (skip). |
| 2026-11-04 | dj:77952c8d8c | shaders | port | 1 | `39c42e645b..77952c8d8c` | picked | db5c71e58a. |
| 2026-11-04 | li#65 | shaders | port | 1 | `546cb67825..d9748a5d58` | adapted | 2b1088e61b, core fix only; CACHE_VERSION GL 23, VK 27; without the 0x986 and register_pressure.h cases (li:b3f983a6b8) and the profile zones. |
| 2026-11-04 | li#64 | services | port | 1 | `d9748a5d58..c6ac0566e9` | adapted | abfd62eddf, the pin stpl:u 0 had no 300 handler, upstream one added whole. |
| 2026-11-04 | li#66 | gpu-core | port | 1 | `c6ac0566e9..fe8d0dda89` | adapted | 999f3830ae, only the buffer form in the pin SURED 0x59f; the pin IMNMX already widens through R250, its test file left out. |
| 2026-11-04 | li#70 | gpu-core | depends | 1 | `b3a015e124..e3d8e9c605` | dropped | needs dj#322 (deferred) and dj:660e5f0039 (measure). |
| 2026-11-04 | dj#492 | shaders | port | 1 | `d6efc8ef08..d2fcf00abe` | picked | e4603d85a0. |
| 2026-11-05 | dj#567 | kernel-memory | port | 1 | `434c07b347..e736dfde9e` | picked | 2cb94d6ea2. |
| 2026-11-05 | li#89 | gpu-vulkan | port | 1 | `c9ebb5c3af..724aede4ea` | picked | 359e115824; vk_texture_cache.cpp anchors match. |
| 2026-11-05 | li#100 | gpu-core | depends | 1 | `724aede4ea..b328477840` | dropped | needs dj#465 or li:b3f983a6b8 shadow copies, not in the port. |
| 2026-11-05 | li#103 | gpu-core | depends | 1 | `b328477840..3165a2b6f3` | dropped | needs dj:b13c4c1c0c, not in the log. |
| 2026-11-05 | li#88 | gpu-vulkan | port | 1 | `a8421cebb0..509809ddf4` | picked | faa5b25a6c; vk_swapchain.cpp anchors match. |
| 2026-11-05 | li#110 | gpu-core | depends | 5 | `8969116854..62ba83427e` | partial | 7b953db74e empty (the pin PaceToVblank has no per-process MKW check); 703da11d1e (#543) adapted as b8638b2d0f onto the pin Nx2GpuCommandScope maps; ec3a455c6f as 17b0dd66eb; 0239f29a79 as bf27154e5f; swapchain 6b7c516b09 left out. |
| 2026-11-05 | li#112 | shaders | depends | 1 | `30d6061f5b..b8c812b186` | deferred | 14 conflict blocks against the pin TLD4 case (0xb63), written on lichii own TLD4 copy; a hand port of its DC, F16 per-tap and offset-proof forms is past the time-box (OoT upscaler dispatches stay dropped, no crash). |
| 2026-11-05 | li#121 | gpu-vulkan | port | 1 | `99ac3d5955..5b89a3daf4` | picked | 65b732477d; vk_scheduler.cpp and vk_query_cache.cpp anchors match. |
| 2026-11-05 | li#142 | gpu-vulkan | port | 1 | `90e2d85bb3..fe6a388a4b` | adapted | 3e77e72dcc, the check applied by hand to the pin draw lambdas. |
| 2026-11-05 | li#119 | kernel-memory | depends | 1 | `77cbc3e7c3..0db1cdbd8b` | picked | e5db65929c (dj#349 is adapted). |
| 2026-11-05 | li#138 | frontend-build | port | 1 | `7c0b5e791e..7e1d645557` | picked | 19d61164d6; the eden-cli half is frontend only. |
| 2026-11-05 | dj#639 | gpu-core | depends | 1 | `778357c26f..7d9823a601` | dropped | needs dj:b13c4c1c0c and dj:660e5f0039, not in the log. |
| 2026-11-05 | li#150 | shaders | depends | 1 | `7e1d645557..f26ad432ae` | adapted | e34a938a8d; kept the pin flush and native sizing, table pointers sized by mapping, traces dropped, CACHE_VERSION GL 24 VK 28, port Idle anchor updated. |
| 2026-11-05 | li#144 | gpu-core | depends | 1 | `f26ad432ae..f356d39ba3` | dropped | needs li:4a58842d23, dropped. |
| 2026-11-05 | li#140 | shaders | depends | 1 | `f356d39ba3..a6e5217c7b` | dropped | needs li:ca6b8e3ca0, dropped. |
| 2026-11-05 | li#141 | gpu-core | port | 1 | `a6e5217c7b..27226d2568` | picked | 520dc0dfea. |

## Pull requests

Open pull requests ported after the rows above, in the coordinator's order, from the triage in [prs/](prs/README.md). Unit is `dj-pr#N` (Djihads80/Eglantine) or `li-pr#N` (zortos293/Lichii); File is the triage file; Diff lists the commits taken.

| Date | Unit | File | Verdict | Commits | Diff | Status | Notes |
|---|---|---|---|---|---|---|---|
| 2026-11-02 | dj-pr#329 | djihads80-old | port | 1 | `0a41327ff3` | partial | 06275fe25d, only the kind_map lock in PageTableOp. |
| 2026-11-05 | dj-pr#462 | djihads80-old | port | 2 | `4c0cbe1b0e d1f6ead1b8` | partial | 63cb1edfaa, 985d5d7122; without the AVX-512, core.cpp, core_timing, SDL and test commits. |
| 2026-11-04 | dj-pr#502 | djihads80-old | port | 1 | `aec509f48a` | partial | c44d1d4925, only the null-image ScaleDown fix; the NV12 commits left out. |
| 2026-11-05 | dj-pr#129 | djihads80-old | port | 2 | `c3b5312e20 56cc21e230` | partial | 0ab52d78e2, c6544834c9; 4f950a7790 left out (it would break the fiber.cpp rewrite). |
| 2026-11-06 | dj-pr#503 | djihads80-old | port | 4 | `c520ca6d85 9c2f4dbc6c 5c26200815 f5163e8292` | partial | 4c7c5e88dc, 7591353479, 0cb835de45, 9de3c4eb05; 792daba207 already in v2. |
| 2026-11-04 | dj-pr#534 | djihads80-new | port | 1 | `34bf6746b3` | adapted | 68033c610e, the button-glyph merge redone on v2's plain-TTF builtin_font at load; carry 7860296bcc leaves out its test, which needs be2f49ed43's synthesized font. |
| 2026-11-05 | dj-pr#646 | djihads80-new | port | 1 | `c6cf43efe8` | adapted | eeb54fb73b, RequestLoadNxFont copies from builtin_font at builtin_font_regions. |
| 2026-11-05 | dj-pr#613 | djihads80-new | port | 1 | `08d9b0b619` | picked | 775e8e1c96, after dj#567. |
| 2026-11-06 | li-pr#206 | lichii | port | 2 | `22f0214c7a 2b5cfdd0b9` | picked | 466df31162, 581a447f4b; without #193's word_manager.h hunk. |
| 2026-11-05 | li-pr#173 | lichii | port | 1 | `ee139bdeef` | adapted | 06d28c6f81, texture_pass.cpp only: v2 has no ampere_logged_textures.cpp, so the test hunk is left out. |
| 2026-11-06 | li-pr#185 | lichii | port | 2 | `99c5b39c9e 7e2a6a78f8` | picked | 4fa1a507f8, 0070e70823; kept patch 0004's LoadingProbe mark and PrepareSparseTilesForWrite beside the flushes. |
| 2026-11-06 | li-pr#159 | lichii | port | 1 | `4df372950c` | picked | a5f0c609ec. |
| 2026-11-06 | li-pr#204 | lichii | port | 1 | `0bdc4ad004` | dropped | not needed: li#150 kept the pin's flush, so ReadNativeMemory and FrontDmaRead still flush the query cache before macro DMA reads under the split; its hunks edit li#70's FrontDmaRead and li#150's split-gated flush, which v2 does not have. |
| 2026-11-05 | li-pr#176 | lichii | port | 1 | `dbcbe547b3` | picked | 33d62abeb1, after li#150. |
| 2026-11-06 | li-pr#192 | lichii | port | 1 | `b8e8e0a5c0` | adapted | c35e706b66, task and mesh half only: the mesh skip dropped from SerializeGraphicsPipeline and LoadDiskResources; no trap_prefix in v2. |
| 2026-11-06 | li-pr#197 | lichii | port | 2 | `115cc6a20b 237ab47263` | deferred | returning null for a rejected program drops the rethrown matrix-multiply error that KeplerCompute::DispatchNativeCompute needs for v2's UpscaleInsteadOfNativeProgram stand-in (585e457172) after the first dispatch; needs that stand-in keyed on the rejection first. |
| 2026-11-06 | li-pr#189 | lichii | port | 1 | `619c7d04ea` | dropped | does not stand without li-pr#149: it edits register_pressure.h and the shadow check of li:b3f983a6b8, which v2 lacks (the pin rejects those registers). |
