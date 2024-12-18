cd dotf_enc_data
srec_cat.exe ..\Debug\dotf_rsip_protected_mode_ek_ra8m1.srec -crop 0x00000000 0x7FFFFFFF -o ra_app_image_wo_ospi_area.srec
srec_cat.exe ..\Debug\dotf_rsip_protected_mode_ek_ra8m1.srec -crop 0x80000000 0x9FFFFFFF -o ra_app_image_ospi_area.srec
dotf_cli_v107\skmt.exe /encdotf /keytype "AES-128" /enckey "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF0" /nonce "00000000000000000000000000000009" /startaddr "90000000" /endaddr "90000FFF" /prg "./ra_app_image_ospi_area.srec" /incplain /output "./ra_app_image_ospi_area_encrypted_and_plaintext.srec"