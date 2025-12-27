cd dotf_enc_data
srec_cat.exe ..\Debug\doft_rsip_protected_mode_ra8p1_CPU1.srec -crop 0x00000000 0x6FFFFFFF -o ra_app_image_wo_ospi_area_cpu1.srec
srec_cat.exe ..\Debug\doft_rsip_protected_mode_ra8p1_CPU1.srec -crop 0x80000000 0x9FFFFFFF -o ra_app_image_ospi_area_cpu1.srec
dotf_cli_v110\skmt.exe /encdotf /keytype "AES-128" /enckey "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0" /nonce "00000000000000000000000000000009" /startaddr "90004000" /endaddr "90005FFF" /prg "./ra_app_image_ospi_area_cpu1.srec" /incplain /output "./ra_app_image_ospi_area_encrypted_and_plaintext_cpu1.srec"
