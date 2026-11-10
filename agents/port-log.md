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

## Upstream sync, batch S1a

Stage 7 of [PLAN.md](../PLAN.md#upstream-sync-2026-11-10), in `v2`'s commit order. Unit is `ps5 patch 00NN` (a core patch of heydemoura/EglantinePS5 at `095df4e`) or `ps5-pr#N` (the pull request's commits outside the core); File is the triage file in [prs/](prs/); Diff is the EglantinePS5 commit taken. The hashes in Notes are `v2`'s own. The core commits were made in `/Volumes/Hangar/ps5/scratch/triage-20261110/work` (branch `sync`) and came into `v2` with `git format-patch -k | git am -k --directory=Eglantine`. Patches 0024 and 0033 and #68 change behaviour and came with batch S1b, below.

| Date | Unit | File | Verdict | Commits | Diff | Status | Notes |
|---|---|---|---|---|---|---|---|
| 2026-11-10 | ps5 patch 0025 | eglantineps5-main | port | 1 | `ced139a` | picked | ba7ce09. |
| 2026-11-10 | ps5 patch 0034 | eglantineps5-main | port | 1 | `3c7bcba` | picked | e98c660. |
| 2026-11-10 | ps5 patch 0035 | eglantineps5-main | port | 1 | `15757b6` | picked | c721e11. |
| 2026-11-10 | ps5 patch 0036 | eglantineps5-main | port | 1 | `206dc0d` | picked | d7cb310. |
| 2026-11-10 | ps5 patch 0047 | eglantineps5-main | port | 1 | `4d11c49` | adapted | 92aa77c, its test case goes into ampere_compute_lowering.cpp: v2 has no ampere_pokopia_compute.cpp. be3a1de bumps CACHE_VERSION to Vulkan 32 and GL 28. |
| 2026-11-10 | ps5 patch 0019 | eglantineps5-main | port | 1 | `39d36b1` | picked | ce49770. |
| 2026-11-10 | ps5 patch 0058 | eglantineps5-main | port | 1 | `5e1e654` | adapted | d143cbc, the catch block's reset comes after v2's build-failure log, which reads the modules. Alone it frees the modules into the heap; nothing returns to the pool without #84's arena release. |
| 2026-11-10 | ps5 patch 0055 | eglantineps5-main | port | 1 | `47b54bb` | picked | c5666e9; ReportOpenFiles has no caller on v2. |
| 2026-11-10 | ps5-pr#90 | eglantineps5-port | port | 1 | `47b54bb` | partial | 7e115e3, only headless/main.cpp's MESA_DISK_CACHE_DATABASE_NUM_PARTS=4; the descriptor census in performance.cpp and nfs/nfs.cpp is left out. |
| 2026-11-10 | ps5-pr#75 | eglantineps5-port | port | 1 | `03e3341` | dropped | its only commit outside the core edits tools/patch-radv-pieces.py and tools/patch-radv-trim.py, which come with #69 and are not in v2. |

## Upstream sync, batch S1c

Units of [delta/](delta/), in `v2`'s commit order, ported the same way (scratch branch `s1c-final`). Each commit keeps its upstream author and subject.

| Date | Unit | File | Verdict | Commits | Diff | Status | Notes |
|---|---|---|---|---|---|---|---|
| 2026-11-10 | dj:9b27d7eac8 | shaders | port | 1 | `61bfe3dce6..9b27d7eac8` | adapted | 4655956, v2's F2I arm kept: the constant-buffer abs and neg bits now reach the load on the plain word form too (it accepted and dropped them), the U16 form already applied them, byte and extended-word still reject them; no cache version bump (S1a's 32/28 covers it); ampere_logged_numeric.cpp is not in v2; upstream's test added to ampere_f2i.cpp. |
| 2026-11-10 | dj:d713f4462a | common | port | 1 | `abb9ae8426..d713f4462a` | picked | 85340e5. |
| 2026-11-10 | dj:1b7995d89c | gpu-core | port | 1 | `5778922694..1b7995d89c` | picked | 71c8415. |
| 2026-11-10 | dj:1725659895 | gpu-core | port | 1 | `1b7995d89c..1725659895` | picked | a44c7c5, beside dj#562's whole-buffer skip; EglantinePS5 patch 0063 does not touch word_manager.h, patch 0064 carries the same early-out (with a comment). |
| 2026-11-10 | li#414 | gpu-vulkan | port | 1 | `2b70fc5ee9..0a619eaaa7` | picked | 3be9ef2, 61f743c takes the `vk_buffer_cache` wait-site count in `tools/prepare-vulkan-port.py` from 6 to 5. |
| 2026-11-10 | li#346 | gpu-core | port | 1 | `2ae4c3e402..047ab1d012` | picked | 85f6241, tests/CMakeLists.txt list merged. |
| 2026-11-10 | li#353 | gpu-vulkan | port | 1 | `7223507e47..79c151694a` | partial | 60b9452, only InvalidateState() after the bare-view BlitColor in blit_image.cpp; the colour write-mask half left out, as the Why says. |
| 2026-11-10 | li#311 | gpu-core | port | 1 | `077e455c85..aa0acb3ede` | picked | c363067. |
| 2026-11-10 | li#435 | kernel-memory | port | 1 | `331f53d107..4e9c4cb4a1` | partial | 1621b8e, memory.cpp only: the k_address_arbiter.cpp half is in v2 as 775e8e1c96 and tests/core/memory_acquire.cpp is not in v2. |
| 2026-11-10 | li#444 | gpu-vulkan | port | 1 | `4e9c4cb4a1..57a6334ec4` | picked | 6d2871e, TickFrame keeps v2's readback_writers.Prune() beside the new drain. |
| 2026-11-10 | li#427 | gpu-vulkan | port | 1 | `b6d3a82eed..6c9c06cc78` | picked | 516cab7, tests/CMakeLists.txt list merged. |
| 2026-11-10 | li#580 | gpu-core | port | 1 | `931c9a649b..7083185c43` | picked | 175c565, tests/CMakeLists.txt list merged. |
| 2026-11-10 | li#700 | gpu-core | port | 1 | `59514b196e..821ccd570a` | adapted | 352c713, accumulator half as upstream; query half is the removal of `return result;` from FlushRegion's callback (v2's VisitQueryRegion already has the right trait, so the walk stopped at the first query waiting for the host); IterateCache unchanged. |
| 2026-11-10 | li#600 | services | port | 1 | `e7e5fef1fc..455f635a49` | partial | 961ce23, IAudioRenderer command 14 only; nx2_audio_user_client.h unchanged. |
| 2026-11-10 | li#561 | services | port | 1 | `455f635a49..0dd7c1d554` | picked | 259fa33. |
| 2026-11-10 | li#727 | services | port | 1 | `88783d558d..c0f8098422` | partial | ab242d2, nifm IGeneralService 101 only; nx2:vi2 201 stays RegisterClientState. |
| 2026-11-10 | li#699 | gpu-vulkan | port | 1 | `d3cbd3b293..ac166ebc64` | picked | c23bf17. |
| 2026-11-10 | li#296 | gpu-core | port | 1 | `7cafadfd7d..d590d51820` | picked | 228ffac. |
| 2026-11-10 | li#878 | gpu-core | port | 1 | `bbf3e62974..fd4ce24c9b` | dropped | ported by hand in the scratch clone and left out: it does not apply to v2's pipeline cache, the hand port had to free the IR on the NIR compute path too, and nothing measures its gain. |
| 2026-11-10 | li#918 | common | port | 1 | `1c25fbdfb4..281477eac7` | picked | 706940d. |
| 2026-11-10 | li#957 | common | port | 1 | `cbb512ee7a..cee0c2dbc0` | picked | 6eef87a, tests/CMakeLists.txt list merged. |
| 2026-11-10 | li#984 | kernel-memory | port | 1 | `b29ef4b583..0fff860be6` | picked | c40f806. |
| 2026-11-10 | li#1196 | audio | port | 1 | `d2eaae4f3a..314e1d9469` | partial | e24d773, the decode.cpp fix; tests/audio_core/adpcm_decode.cpp left out, it needs tests/core/system_test_context.h, which v2 does not have. |

## Upstream sync, batch S2

The Mario Kart World stall ([mkw-hang-20261110.md](mkw-hang-20261110.md)).

| Date | Unit | File | Verdict | Commits | Diff | Status | Notes |
|---|---|---|---|---|---|---|---|
| 2026-11-10 | dj:8e87cf5a66 | gpu-vulkan | port | 1 | `0ca2032bff..8e87cf5a66` | partial | 1e26c76, the shader part only (EmitMeshPrologue stores zero in the task payload's mesh workgroup count), behind `EDEN_NX2_TASK_COUNT_INIT=0`; the Intel paths are left out. |

Beside it, this port's own commits: ecd174c caps the count a task program emits at `maxMeshWorkGroupCount` (`EDEN_NX2_MESH_GROUP_LIMIT=0` restores), 2726a1f records the draws and dispatches behind a stalled submission (`EDEN_NX2_DRAW_RING=1`, `EDEN_NX2_STALL_MS=<ms>`), 7727fd7 closes the app after a crash report (`EDEN_CRASH_NO_RESTART=1`).

## Upstream sync, batch S1b

The behaviour changes held out of build 2. Ported in `/Volumes/Hangar/ps5/scratch/triage-20261110/work-s3` (branch `s3`).

| Date | Unit | File | Verdict | Commits | Diff | Status | Notes |
|---|---|---|---|---|---|---|---|
| 2026-11-10 | ps5 patch 0024 | eglantineps5-main | port | 1 | `1db8efc` | picked | a46e7f0. |
| 2026-11-10 | ps5 patch 0033 | eglantineps5-main | port | 1 | `548eba4` | picked | baa691a. |
| 2026-11-10 | ps5-pr#68 | eglantineps5-port | port | 1 | `e01b2d9` | picked | 7c9a68c; the derived k_memory_manager.cpp and k_page_heap.cpp are built for the first time: free return on (`EDEN_GUEST_FREE_RETURN=0` restores), commit on hand-out live with no switch (a no-op while the guest memory service runs), the fill rule still not called on this core. |
| 2026-11-10 | li#659 | gpu-core | port | 1 | `72e9429daf` | partial | e27508b, the fsp half only (`fs_i_storage.cpp`): the read throttle is skipped for Switch 2 titles. No switch. |
| 2026-11-10 | li#436 | gpu-core | port | 1 | `32886f755c` | picked | 54b7df7. `src/tests/CMakeLists.txt` takes only `video_core/native_qmd.cpp`. |

## Upstream sync, batch S3

The DK Bananza draw path, same clone, in `v2`'s commit order.

| Date | Unit | File | Verdict | Commits | Diff | Status | Notes |
|---|---|---|---|---|---|---|---|
| 2026-11-10 | dj:47b4169b3f | gpu-core | port | 1 | `47b4169b3f` | adapted | 3b1c264. `map_serial` starts at 1 and is bumped after `v2`'s locked `kind_map.Map` in `PageTableOp` and `BigPageTableOp`. |
| 2026-11-10 | dj:292a68df46 | gpu-core | port | 1 | `292a68df46` | picked | e58b451. |
| 2026-11-10 | dj:c8415d0e80 | gpu-core | port | 1 | `c8415d0e80` | adapted | e9d1e6c. `StorageBufferBinding` has no `native_pointer` here: the cache key uses `table_pointer`, and `v2`'s 8 MiB cap for table pointers stays. Adds `MemoryManager::MapSerial()`. |
| 2026-11-10 | ps5 patch 0029 | eglantineps5-main | port | 1 | `548eba4` | adapted | 84aa7ab. `DescriptorTable` keeps `v2`'s lazy `invalidated` clear and one-page read; takes `bound_ids` and the power-of-two growth. |
| 2026-11-10 | ps5 patch 0026 | eglantineps5-main | adapt | 1 | `ced139a` | adapted | 6aeecb2. Release tracking owns the acquires by default; QMD releases follow li#436 (inexact ones noted as pending or unknown). `EDEN_NX2_ACQ_LEDGER=1` runs `v2`'s ledger instead (tracking off), `=0` turns both off. `v2`'s `sent_releases`/`AwaitReleases` stay. |
| 2026-11-10 | ps5 patch 0027 | eglantineps5-main | measure | 1 | `ced139a` | adapted | b45a5ad. Read-ahead runs only with 0026's tracking; `EDEN_NX2_DMA_READ_AHEAD=0` (or `EDEN_NX2_ACQ_LEDGER=0/1`) drains for every read as before. `EDEN_NX2_DMA_READ_TRUST=1` is opt-in. |
| 2026-11-10 | ps5 patch 0028 | eglantineps5-main | measure | 1 | `548eba4` | picked | 3c3c424. Only the `gpu_split.h` member lists merged by hand. `EDEN_NX2_REG_BLOCKS=0` sends writes one by one. |
| 2026-11-10 | ps5 patch 0018 | eglantineps5-main | adapt | 1 | `1ebcdf5` | partial | b966a80. Feedback copy coalescing (`EDEN_NX2_FEEDBACK_COPIES=exact` restores) and the `overlap_epoch` walk. `alias_maybe_newer` left out (`v2`'s `alias_modification_tick` does it); the vertex sync cache left out (`HasPendingCpuUpload` covers it). |
| 2026-11-10 | ps5 patch 0061 | eglantineps5-main | measure | 1 | `573b76a` | adapted | 2d40a05. `v2`'s `overwritten_by_clear` skip stays, tested after the new one. Added switch `EDEN_NX2_JOIN_WHOLE_COPY=0`. |
| 2026-11-10 | ps5 patch 0062 | eglantineps5-main | adapt | 1 | `573b76a` | adapted | f0630d9. `Nx2MayBeGpuWritten` reads `g_nx2_gpu_written_pages` (buffer cache writes only in `v2`); the second bitmap and `Nx2MarkBufferWritten` left out; the test list keeps its entries. ebed548 adds the switch `EDEN_NX2_STALE_SKIP=0`. |
| 2026-11-10 | li#250 | gpu-core | port | 1 | `a195b27ffe..e67d389592` | adapted | a5202d6. The pipeline cache has no `InvalidateRegions` here: it is invalidated range by range behind the same page filter test. |
| 2026-11-10 | li#611 | gpu-core | port | 1 | `23e91fa495..5b2df34e7b` | picked | b3b442c. |
| 2026-11-10 | ps5 patch 0030 | eglantineps5-main | measure | 1 | `548eba4` | picked | c6ffe78. `EDEN_NX2_UNIFORM_PREFETCH=0` turns it off. |
| 2026-11-10 | ps5 patch 0063 | eglantineps5-main | adapt | 1 | `9f783da` | adapted | 9107849 (Lichii #858, #921, #910), ported in the main thread after the batch. Beyond the six conflicts: the patch's second `map_serial` is dropped for the one dj:47b4169b3f added; `SamplingChangesImage` reads `alias_modification_tick`; views `ReplaceColorFeedbackViews` pointed at a copy are replaced again on a memo hit and block the delta capture; pipelines with `handle_program` descriptors take neither the memo nor delta draws; `ConfigureDelta` passes `desc.indirect_offset`; the `before_later_reads` command in `gpu_thread.cpp` ends the chain. 7a72b13 stubs the breaks in `tools/check-gpu-sync-stop.py`. `EDEN_NX2_FAST_DRAW=0` and `EDEN_NX2_TEXTURE_MEMO=0` turn them off, the `_VERIFY=1` ones count mismatches. |

## Upstream sync, batch S4

Ocarina of Time and the unmeasured games. Ported by a subagent in `/Volumes/Hangar/ps5/scratch/triage-20261110/work-s4` (branch `s4`), then rebased onto S3 and 0063 in `work-s3` (branch `final`) with two conflicts: li#351 keeps S3's cached storage buffer binding and changes only the size rule, and li#778's null texture read sits inside 0063's full resolve loop. The lowering harness (`/Volumes/Hangar/ps5/scratch/triage-20261110/s4/harness`) went from 117 pass, 26 fail, 1 crash to 141 pass, 13 fail. The shader cache versions are 33 and 29: every game's cache starts empty on the first run. Compiled by the final build (249 test cases pass); not run on the console.

| Date | Unit | File | Verdict | Commits | Diff | Status | Notes |
|---|---|---|---|---|---|---|---|
| 2026-11-10 | dj:350b7d8ffd | shaders | port | 1 | `b6706b0c29..350b7d8ffd` | adapted | 7c8dd39. ISETP.EX keeps v2's plain form and takes upstream's temporary-predicate path for the accumulating forms; covers part of patch 0068. |
| 2026-11-10 | dj:3f065ef733 | shaders | port | 1 | `350b7d8ffd..3f065ef733` | adapted | d6f6025. UIMAD with a constant buffer (0x4a4), one-source F2IP.U8, negated F2I, SHF.R.U64 by an immediate; 0x8a5 was in v2. v2's ampere_f2i.cpp and ampere_release_f2i.cpp asserted that a negated F2I source is rejected: their rejected modifier is now bit 61. Covers 0x4a4 of patch 0054. |
| 2026-11-10 | dj:223c752f44 | shaders | depends | 1 | `179dbdbb63..223c752f44` | picked | c71f05b. Comes with dj:3f065ef733 (it removes the U64 clamp that commit added); its test is in ampere_lowering.cpp. |
| 2026-11-10 | dj:bb5998f497 | shaders | port | 1 | `3f065ef733..bb5998f497` | adapted | bdd2549. Taken in the lowering group at its upstream position, not first in the fixes group: its forms (SHF/USHF.R.U64, guarded USEL, UFLO/UBREV/UPOPC through the high mirrors, CALL.REL.NOINC counted as a call in program_capture.h) are the 0068/0069 forms the harness checks, and dj:edf4eacc7d builds on it. v2's ampere_mesh.cpp test follows (UPOPC of UR16 and up now lowers). |
| 2026-11-10 | dj:edf4eacc7d | shaders | port | 1 | `bb5998f497..edf4eacc7d` | adapted | 056f155. UISETP with a UPn guard and uniform SEL (0xc07), in v2's handlers. |
| 2026-11-10 | dj:f1a5c6e04f | shaders | port | 1 | `edf4eacc7d..f1a5c6e04f` | partial | 2f2d9cc. Only the plain 2D-array TLD4; v2's depth-compare and per-tap layouts stay, and three assertions of upstream's test follow v2's per-pixel layout. |
| 2026-11-10 | dj:19e25d0cfb | shaders | port | 1 | `f1a5c6e04f..19e25d0cfb` | partial | 9083e51. Only the UIADD3 carry limit (a carry to or from any UPn); the other forms were in v2, and SURED keeps v2's ADD and OR only. |
| 2026-11-10 | 0054 ocarina-compute-shaders (lowering) | eglantineps5-main | adapt | 1 | `tools/eglantine-patches/0054-ocarina-compute-shaders.patch` | partial | 5ddbe1d. The two UIADD3 forms with UPn carries, placed after v2's single-carry block so that those forms keep their lowering. 0x8a5 was in v2, 0x4a4 came with dj:3f065ef733, the software HMMA default stays opt-in. The harness has no case for these two forms: checked by reading the emitted words. |
| 2026-11-10 | 0068 ocarina-gameplay-shaders | eglantineps5-main | adapt | 1 | `tools/eglantine-patches/0068-ocarina-gameplay-shaders.patch` | partial | 5cdf57b. MUFU of a negated or absolute uniform register, with the patch's ampere_kirby.cpp case. Deferred: the TEX path for a handle in a uniform register outside the handle-extract pattern (the patch enters its general TEX block with a goto; v2's block is a different lambda and needs a rewrite proven against the texture pass) and the constant-offset proof of half per-tap gathers across an unrelated LEA.HI (the patch's per-tap gather is another implementation than v2's). Its other lowering forms came with the dj units above; its SPIR-V and register-bank parts were not in the S4 list. |
| 2026-11-10 | 0069 ampere-high-uniform-registers | eglantineps5-main | adapt | 0 | `tools/eglantine-patches/0069-ampere-high-uniform-registers.patch` | dropped | No commit: v2 already mirrors UR28-UR62, and the handlers the patch widens came with dj:bb5998f497 (UFLO, UBREV, UPOPC) and dj:edf4eacc7d (UISETP guard, uniform SEL). The harness's high-uniform cases pass. |
| 2026-11-10 | cache version | - | - | 1 | - | picked | c4fd7e8. Vulkan 32 to 33, GL 28 to 29, once for the whole batch: the fixes after it (64 compute storage buffers, mesh topologies, byte ULDC) also change generated code, so the batch must ship as a whole or be bumped again. |
| 2026-11-10 | dj:9a69d8eaf0 | shaders | port | 1 | `b213a58ab5..9a69d8eaf0` | partial | f638dce. ULDC.U8/U16 and the two-source F2IP.U8, ungated (no EDEN_NX2_LOWER_EXTENDED); the ULEA carry to any UPn and I2F.U8 of a UR byte were in v2; its cache version lines are left to c4fd7e8. |
| 2026-11-10 | dj:de4ed9d9d7 | shaders | port | 1 | `900ab5a825..de4ed9d9d7` | adapted | 08c16e8. Touches the buffer cache (S3's area): `buffer_cache.h` two lines, `buffer_cache_base.h` counts and masks. NUM_STORAGE_BUFFERS names MAX_GRAPHICS_SSBOS and NUM_COMPUTE_STORAGE_BUFFERS MAX_SSBOS, v2's mask assertion becomes two (u32 graphics, u64 compute) and v2's compute_storage_buffers.cpp test uses u64 masks. |
| 2026-11-10 | dj:743d5bbaed | shaders | port | 1 | `7d26d3b2fb..743d5bbaed` | adapted | 1c20003. MakeRuntimeInfo keeps v2's depth mode from the runtime key and adds the point size. |
| 2026-11-10 | dj:900ab5a825 | shaders | port | 1 | `9afdebafbd..900ab5a825` | partial | 5ccebd7. One line: SHF 0x219 in `register_pressure.h`; the other lists were in v2. |
| 2026-11-10 | li#241 | shaders | port | 1 | `79f284eab8..f2960358b8` | partial | 0d965f1. The LOP3.LUT shadow list (0xa12, 0xc12). The retry is left out: v2 shadows from a fixed, aligned R216 and rejects a native register inside the high mirrors, so there is no misaligned base; ampere_ported_uniform_vote.cpp is not in v2. |
| 2026-11-10 | li#279 | gpu-core | port | 1 | `047ab1d012..c149054446` | adapted | 5d2d744. 0xc24 beside 0xa24; ampere_logged_numeric.cpp is not in v2. |
| 2026-11-10 | li#588 | shaders | port | 1 | `760183a2ad..33acb0e1a0` | partial | fcbad9a. The ATOMG and TXD shadow lists; v2's scratch check already counts their bit 32 operand, v2 has no TXD.B (0xf6c) handler, the test file is not in v2. |
| 2026-11-10 | li#610 | gpu-core | port | 0 | `12fc47e8aa..4c63e74e19` | dropped | Already in v2 in its own form: v2's half-precision gather wrapper packs only the enabled taps and pairs an odd last tap with RZ (masks 1, 2, 3, 12 and 3 or 7 with a second destination lower, 7 and 15 without one are rejected, as upstream's test expects; checked with `/Volumes/Hangar/ps5/scratch/triage-20261110/s4/harness/adhoc/t3.cpp`). |
| 2026-11-10 | li#398 | shaders | port | 0 | `f06e901a54..54dedf97b6` | deferred | TLD4.B from a second descriptor buffer is written against lichii's gather emission; v2's differs in every mode (depth compare, per-pixel offsets, arrays), so each mode has to be rewritten and proven with the translator, and v2 has no ampere_ported_textures.cpp to carry the test. |
| 2026-11-10 | li#337 | gpu-core | port | 0 | `d181de503c..673783fa6b` | deferred | With li#215. Alone it keeps fence-flagged reports pending while v2's macro DMA reads only flush without waiting, so they would read stale reports more often (Mario Tennis Fever's occlusion-driven characters). |
| 2026-11-10 | li#215 | gpu-core | port | 0 | `5ac8573642..7ae3e74014` | deferred | S3's area (macro and DMA reads). v2 has no query_dma.h and its two sites flush the buffer and query caches together through the memory manager (macro.cpp, and maxwell_3d.cpp on the front thread with the back drained). `FlushQueryResults` blocks the caller until an operation queued behind the fence has run: which thread releases that fence when the caller is v2's front thread was not established, and a wrong answer is a hang, so it needs a build and a console run. |
| 2026-11-10 | li#439 | gpu-core | port | 1 | `57a6334ec4..7969a392c2` | adapted | 0c37c63. turing_macro_ram.cpp taken and listed in tests/CMakeLists.txt beside v2's tests. |
| 2026-11-10 | li#718 | gpu-vulkan | port | 1 | `6e292be6b5..88783d558d` | adapted | 9f6d1ea. The bound (4 banks) and the shader comparison; v2 has no drop counter or warning there. |
| 2026-11-10 | dj:688c6fa7d8 | gpu-vulkan | port | 1 | `58187e9cb2..688c6fa7d8` | adapted | c6792cc. The rebuild check runs before the early return of CurrentComputePipeline, which keeps that anchor of `tools/prepare-vulkan-port.py`; the new `previous` parameter breaks the tool's `CreateComputePipeline` timer anchor: `/Volumes/Hangar/ps5/scratch/triage-20261110/s4/anchors.patch` (one tuple). |
| 2026-11-10 | li#351 | gpu-opengl | port | 1 | `ac4ed63ad1..b72195422b` | adapted | 208ec3f. Touches the buffer cache (S3's area). Applied to v2's StorageBufferBinding (no binding cache around it). The gl_* half is taken too: the template calls `runtime.GetMaxStorageBufferRange()` for both runtimes and the port has an EDEN_PS5_OPENGL build; it merged without conflict. |
| 2026-11-10 | dj:fe2cd8be2b | gpu-core | port | 0 | `87bd90ae35..fe2cd8be2b` | deferred | S3's area. Six files conflict in ten regions (the buffer cache's download paths, gpu.cpp, the rasterizer interface, vk_rasterizer), and only its download half is wanted: separating the halves by hand cannot be checked without a build. |
| 2026-11-10 | li#778 | gpu-core | port | 1 | `aae0b063f7..794602bfbd` | partial | 852ad29. The texture handle part (`d603d98bb3` is not an object in the clones; taken from the squashed commit): shader_handle_program.h, vk_compute_pipeline.cpp, vk_graphics_pipeline.cpp. The staging buffer retention, fence profile lines, per-method switch reads, debug tools and tests are left out. |
| 2026-11-10 | li#679 | gpu-vulkan | port | 1 | `ce9be9d14a..bf4c42e223` | partial | a8d1484. Four parts: blend key rebuilt on every Refresh without dynamic blend state, sampler variant without comparison for a plain sample, null 2D texture for a handle past the descriptor pool (with `#include <atomic>` in shader_environment.cpp), empty start when the driver rejects the pipeline cache file. The AMD query wait, the eviction hunk, the uneven block mip rule and docs are left out. |
| 2026-11-10 | #82's setting (0054, core part) | eglantineps5-main | adapt | 1 | `tools/eglantine-patches/0054-ocarina-compute-shaders.patch` | adapted | f7ccf67. oot_network.h and its hooks in kepler_compute, shader_cache and shader_environment, with 0068's list of the nine network passes. On by default, EDEN_OOT_ANTI_ALIASING=game turns it off. It acts on programs that lower: with v2's software HMMA opt-in (EDEN_NX2_SOFT_HMMA=1 unset) the tensor programs are rejected before the bypass is asked, and v2's existing handling of rejected matrix-multiply programs applies instead. The launcher setting is not part of it. |
| 2026-11-10 | 0071 ocarina-skip-mesh-draws | eglantineps5-main | adapt | 1 | `tools/eglantine-patches/0071-ocarina-skip-mesh-draws.patch` | picked | 0825d92. Skips the title's task/mesh draws; EDEN_OOT_MESH_DRAWS=draw issues them. |
| 2026-11-10 | dj:dc96fb139e | shaders | port | 1 | `56881674fe..dc96fb139e` | partial | 266d276. AreOrdered only; control_flow.cpp keeps v2's block lookup. |
| 2026-11-10 | 0038 switch2-movies-av1 | eglantineps5-main | adapt | 1 | `tools/eglantine-patches/0038-switch2-movies-av1.patch` | adapted | 05b255f. codec.cpp, ffmpeg.cpp, nvdec_common.h, nx2_video_decoder.cpp (two additive conflicts: the log block and the NV12 copy width); the nx2_gsv.h hunk was in v2. Port side: `df36026` (EglantinePS5 `9fdc65e` with `f733a61` folded in: THIRD_PARTY_NOTICES.md, tools/build-headless-ffmpeg.sh, tools/deps.json, tools/prepare-build.sh). The final build cross-builds dav1d 1.5.1 and links it (`ff_libdav1d_decoder` is in the symbols ELF); the dav1d configure leaves an `a.out` in the worktree root, delete it. No AV1 movie was played. |
| 2026-11-10 | table pool reach (own) | - | own | 1 | - | new | a4a403b. li#351 binds a read-only table pool up to the device's storage buffer range; `EDEN_NX2_TABLE_POOL_MIB` caps it (default 1024, 64 is the reach before li#351), because the PS5 shares that memory with the guest. |
| 2026-11-10 | timer anchor (own) | - | own | 1 | - | new | 5646e0a. `tools/prepare-vulkan-port.py` follows `CreateComputePipeline`'s new `previous` argument (dj:688c6fa7d8). |
