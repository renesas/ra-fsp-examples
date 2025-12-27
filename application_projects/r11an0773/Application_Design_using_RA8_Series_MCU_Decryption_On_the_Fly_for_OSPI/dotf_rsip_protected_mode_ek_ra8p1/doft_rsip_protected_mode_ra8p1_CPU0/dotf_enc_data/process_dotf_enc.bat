cd dotf_enc_data
srec_cat.exe ..\Debug\doft_rsip_protected_mode_ra8p1_CPU0.srec -crop 0x00000000 0x6FFFFFFF -o ra_app_image_wo_ospi_area_cpu0.srec
srec_cat.exe ..\Debug\doft_rsip_protected_mode_ra8p1_CPU0.srec -crop 0x80000000 0x9FFFFFFF -o ra_app_image_ospi_area_cpu0.srec
dotf_cli_v110\skmt.exe /encdotf /keytype "AES-128" /enckey "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0" /nonce "00000000000000000000000000000009" /startaddr "90000000" /endaddr "90001FFF" /prg "./ra_app_image_ospi_area_cpu0.srec" /incplain /output "./ra_app_image_ospi_area_encrypted_and_plaintext_cpu0.srec"
