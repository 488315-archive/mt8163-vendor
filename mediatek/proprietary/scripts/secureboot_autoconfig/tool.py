from xml.etree import ElementTree as ET
import os

default_sla_public_key_n = "C62AD69D37D29DD02B81D8A34D9D1BF56E2ED8E6FD1DAFDE0BDF28A49E0C305EFA8E93C1903257BE076AE3D13B9F00264337B21682593E74DFD0294E5F3D50159B84A1AD60EDD905D63751A36EBAF810384D66EE594BF515CECEB62866674D3161007ADBCD07C2FF10A3C2B61392CC2614EB4EB7A4C6580EF72A1E51A3F636978DB8AF317903145EF2803C355FB0056EAE53D43F4B4ABFA2B67FDAA04BC35C791C29C617ACD5887B8E772B98A5D30A743EE04F8D164503E4AB7AC0B064C2F87D1B880F91E2CF57A2FCDA54A72B5F7893D418FFB985AF999CB22BA41340E4F586FBC4FB8D3919CB9A539BB6753D15484A494B94FC6E0FAC71D1FC79E1A43BDCAD"
default_daa_public_key_n = "0114A2509E7BAEB8B294751B63E6B72D9F75357ED7090CAC8CD83FDA3E53205D34BD624E2BB2F24AB4104C8248473658847081D7901AF1CE7DAEAAED01F551C18514DE96C3499695DCBB57D02F136062C59110F05CC5586286BA3A597F2EC949D4959BAB3CCC126DDC2D065FB059BB7FD62F07C64F49BBA7D032FCBE2557B15B4D9688941A1B66FE0FEB233F507375F54BB0A0F68D4F98605380092FFA962F27DE24CA437363BA405D51FFEF92571DF09BD162B648A367E4A69D8AB8AB930D00141C4D88AE4E2ADD18F7678C81E3F7283EC5B0AC08DDCCB7424EC1BD51D9A2881C032EECE670369CE78CC59C89CFCCC22E4BDCF202862DF965378B423360DCCF"

default_sbc_private_key_n = "00D57BCF5934CE1A035F4598B42DAD4BC8AE7577FB6D81FA232317E8EE7C4FBB33772EFE378DF7DFE5369BB9ACAB1008FA1CFBA737890012A883B372B15932335B689B46A32F1B383B75F0DE2E1B5B0B9F4E1C3E780C2AB0CD3671EB4E34F30BB4C630A60D168CA124810D0F91A1ACC8EFDCEE52D4762BB35813BCC93878E1D15B750561B78006B4C13A8F76B5F10C941E5776C21192357A9B9D7E02C0FC812D2B154671863DE97CE3ED07F90624A0CADD04079E145168A3558A64786820192F5B638354DA69520288B976296961C337FB18A90120F2B6B365C0E1A57CE4119AE8BC718E08FDA33F1F42AA1C91AED090EC6B5656A66C246F89FDE5FD41A76671"
default_sbc_public_key_n = "01C6B6A1DDF05E818E3DFE16101C5DF65939F352EAB8AACA91CB5BFEC15A1989DD7553343683BC30BB38E45F15BF17BCCAB16A41D695A4318F26504675FE83E92EE21C991C0FBA705395B4A34C331842D8A6F69846B58CC67306E3DE27B05666A6C4372E3FC0D92F314805EDD5B1CB7D25BF3CF9CA9C33C36D97B0B37DA8A44A7A1CA651679D8D680557740C7C1CA25D84BDD12136C2930432808F28265D1E33E667389E4806D865F3CC06329534F7A11861EB688545DCCCEF0B04E96735A08368FA31A1F3260073B31299B216192E620B8D1EA468925ADBD627C49EFC3623658F3CF8AD6D8556272E48FA7711E650287DA19196610F036B6C0D394E42C121D1"


def parse_customization_xml():
	tree = ET.parse("configuration.xml")
	root = tree.getroot()
	
	configs = {}
	#general info
	general = root.find("general")
	configs['general'] = {}
	print "checking general info:"
	
	codebase = general.find("codebase")
	configs['general']['codebase'] = codebase.text.strip()
	print "\tcodebase:" + configs['general']['codebase']
	
	company = general.find("company")
	configs['general']['company'] = company.text.strip()
	print "\tcompany:" + configs['general']['company']
	
	platform = general.find("platform")
	configs['general']['platform'] = platform.text.strip()
	print "\tplatform:" + configs['general']['platform']
	
	project=general.find("project")
	configs['general']['project'] = project.text.strip()
	print "\tproject:" + configs['general']['project']
	
	scatter=general.find("scatter")
	configs['general']['scatter'] = scatter.text.strip()
	print "\tscatter:" + configs['general']['scatter']
	
	storage=general.find("storage-type")
	configs['general']['storage'] = storage.text.strip()
	print "\tstorage:" + configs['general']['storage']

	#feature
	feature = root.find("feature")
	configs['feature'] = {}
	
	#hardware-secure-ctrl
	hardware_secure_ctrl = feature.find("hardware-secure-ctrl")
	configs['feature']['hardware_secure_ctrl'] = {}
	configs['feature']['hardware_secure_ctrl']['Enable_SBC'] = hardware_secure_ctrl.get("Enable_SBC").strip()
	configs['feature']['hardware_secure_ctrl']['Enable_SLA'] = hardware_secure_ctrl.get("Enable_SLA").strip()
	configs['feature']['hardware_secure_ctrl']['Enable_DAA'] = hardware_secure_ctrl.get("Enable_DAA").strip()
	
	#software	-secure-ctrl
	software_secure_ctrl = feature.find("software-secure-ctrl")
	configs['feature']['software_secure_ctrl'] = {}
	configs['feature']['software_secure_ctrl']['Disable_BROM_MODE'] = software_secure_ctrl.get("Disable_BROM_MODE").strip()
	
	#key files
	keyfile = root.find("keyfile")
	configs['keyfile'] = {}
	#hardware keys
	#sbc_key, 2048 key
	sbc_key = keyfile.find('sbc_key')
	if configs['feature']['hardware_secure_ctrl']['Enable_SBC'] == "true" and sbc_key.text.strip() == "AUTO":
		#generate sbc key
		cmdline = "openssl genrsa -out keyfiles/sbc_key.pem 2048"
		os.system(cmdline)
		#update configuration.xml
		sbc_key.text = "keyfiles/sbc_key.pem"
	configs['keyfile']['sbc_key'] = sbc_key.text.strip()
	
	#daa_key, 2048 key
	daa_key = keyfile.find('daa_key')
	if configs['feature']['hardware_secure_ctrl']['Enable_DAA'] == "true" and daa_key.text.strip() == "AUTO":
		#generate sbc key
		cmdline = "openssl genrsa -out keyfiles/daa_key.pem 2048"
		os.system(cmdline)
		#update configuration.xml
		daa_key.text = "keyfiles/daa_key.pem"
	configs['keyfile']['daa_key'] = daa_key.text.strip()
	
	#sla_key, 2048 key
	sla_key = keyfile.find('sla_key')
	if configs['feature']['hardware_secure_ctrl']['Enable_SLA'] == "true" and sla_key.text.strip() == "AUTO":
		#generate sbc key
		cmdline = "openssl genrsa -out keyfiles/sla_key.pem 2048"
		os.system(cmdline)
		#update configuration.xml
		sla_key.text = "keyfiles/sla_key.pem"
	configs['keyfile']['sla_key'] = sla_key.text.strip()
	
	#checking hardware keys
	#checking sbc_key
	if configs['feature']['hardware_secure_ctrl']['Enable_SBC'] == "true" and not os.path.exists(configs['keyfile']['sbc_key']):
		print "sbc_key(%s) does not exist, please check it." % (configs['keyfile']['sbc_key'])
		exit(-1)
	#checking daa_key
	if configs['feature']['hardware_secure_ctrl']['Enable_DAA'] == "true" and not os.path.exists(configs['keyfile']['daa_key']):
		print "daa_key(%s) does not exist, please check it." % (configs['keyfile']['daa_key'])
		exit(-1)
	#checking sla_key
	if configs['feature']['hardware_secure_ctrl']['Enable_SLA'] == "true" and not os.path.exists(configs['keyfile']['sla_key']):
		print "sla_key(%s) does not exist, please check it." % (configs['keyfile']['sla_key'])
		exit(-1)
	
	#software key
	#daa_key, 2048 key
	verified_key = keyfile.find('verified_key')
	if verified_key.text.strip() == "AUTO":
		#generate sbc key
		cmdline = "openssl genrsa -out keyfiles/verified_key.pem 2048"
		os.system(cmdline)
		#update configuration.xml
		verified_key.text = "keyfiles/verified_key.pem"
	configs['keyfile']['verified_key'] = verified_key.text.strip()
	
	#img_key, should be 1024 bytes
	img_key = keyfile.find('img_key')
	if img_key.text.strip() == "AUTO":
		#generate sbc key
		cmdline = "openssl genrsa -out keyfiles/img_key.pem 1024"
		os.system(cmdline)
		#update configuration.xml
		img_key.text = "keyfiles/img_key.pem"
	configs['keyfile']['img_key'] = img_key.text.strip()
	
	#checking software keys, these two keys must exist
	#checking verified_key
	if not os.path.exists(configs['keyfile']['verified_key']):
		print "verified_key(%s) does not exist, please check it." % (configs['keyfile']['verified_key'])
		exit(-1)
	#checking img_key
	if not os.path.exists(configs['keyfile']['img_key']):
		print "img_key(%s) does not exist, please check it." % (configs['keyfile']['img_key'])
		exit(-1)
	
	tree.write("configuration.xml")
			
	return configs
	
#generate_code
def generate_code(configs):
	#software secure boot must be enable in normal case
	#enable ATTR_SBOOT_ENABLE and ATTR_SUSBDL_ENABLE in vendor/mediate/proprietary/bootable/bootloader/preloader/custom/${PROJECT}/${PROJECT}.mk
	project_mk_path = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/%s/%s.mk" % (configs['general']['codebase'], configs['general']['project'], configs['general']['project'])
	lines = ""
	for line in open(project_mk_path).readlines():
		if line.find("MTK_SEC_USBDL=") != -1:
			line = "MTK_SEC_USBDL=ATTR_SUSBDL_ENABLE\n"
		elif line.find("MTK_SEC_BOOT=") != -1:
			line = "MTK_SEC_BOOT=ATTR_SBOOT_ENABLE\n"
		lines += line
	#save project mk
	open("tempfile/%s.mk" % (configs['general']['project']), "w+").write(lines)
	
	#print "generate project mk :" + project_mk_path
	
	#get img auth key
	(img_auth_public_key, img_auth_private_key) = parse_key(configs['keyfile']['img_key'])
	#load template file
	img_auth_key_file = open("template/IMG_AUTH_KEY.ini").read()
	#set keys
	img_auth_key_file = img_auth_key_file.replace("REPLACE_IMG_RSA_N", img_auth_public_key)
	img_auth_key_file = img_auth_key_file.replace("REPLACE_IMG_RSA_D", img_auth_private_key)
	#save keys
	open("tempfile/IMG_AUTH_KEY.ini", "w+").write(img_auth_key_file)
	
	#generate dummy_k2.bin
	dummy_k2_file = open("template/dummy_k2.bin").read()
	#set keys
	dummy_k2_file = dummy_k2_file.replace("REPLACE_IMG_RSA_N", img_auth_public_key)
	dummy_k2_file = dummy_k2_file.replace("REPLACE_IMG_RSA_D", img_auth_private_key)
	#save keys
	open("tempfile/dummy_k2.bin", "w+").write(dummy_k2_file)
	
	#set VERIFIED_BOOT_IMG_AUTH_KEY.ini
	#get verified_key
	(img_verify_public_key, img_verify_private_key) = parse_key(configs['keyfile']['verified_key'])
	#load template file
	img_verify_key_file = open("template/VERIFIED_BOOT_IMG_AUTH_KEY.ini").read()
	#set keys
	img_verify_key_file = img_verify_key_file.replace("REPLACE_VERIFIED_RSA_N", img_verify_public_key)
	img_verify_key_file = img_verify_key_file.replace("REPLACE_VERIFIED_RSA_D", img_verify_private_key)
	#save keys
	open("tempfile/VERIFIED_BOOT_IMG_AUTH_KEY.ini", "w+").write(img_verify_key_file)
	
	#set oemkey.h
	img_verify_public_key_splited = ""
	count = 0
	for ch in img_verify_public_key:
		if count % 32 == 0 and count != 0:
			img_verify_public_key_splited = img_verify_public_key_splited + "                          "
		
		if count % 2 == 0:
			img_verify_public_key_splited = img_verify_public_key_splited + "0x"
			
		img_verify_public_key_splited = img_verify_public_key_splited + ch
		
		 
		if (count+1) % 2 == 0 and (count+1) != len(img_verify_public_key):
			img_verify_public_key_splited += ","
		
		if (count+1) % 32 == 0 and (count+1) != len(img_verify_public_key):
			img_verify_public_key_splited += " \\\n"
			
		count += 1
		
	#load oemkey.h template
	oemkey_file = open("template/oemkey.h").read()
	
	#set key
	oemkey_file = oemkey_file.replace("REPLACE_VERIFIED_RSA_N", img_verify_public_key_splited)
	#save keys
	open("tempfile/oemkey.h", "w+").write(oemkey_file)
	
	#generate cust_sec_ctrl.h
	#this file may be changed, 
	#load cust_sec_ctrl.h
	cust_sec_ctrl_file_path = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/%s/inc/cust_sec_ctrl.h" % (configs['general']['codebase'], configs['general']['project'])
	#find public key
	oem_pubk_found = 0
	ori_public_key = ""
	for line in open(cust_sec_ctrl_file_path).readlines():
		if line.find("define OEM_PUBK") != -1 and line.find("\\") != -1:
			oem_pubk_found = 1
			index = line.find("0x")
			ori_public_key += line
			continue
		
		if oem_pubk_found == 1:
			ori_public_key += line
			if line.find("\\") == -1:
				break
	
	cust_sec_ctrl_file = open(cust_sec_ctrl_file_path).read()
	#replace oem key
	cust_sec_ctrl_file = cust_sec_ctrl_file.replace(ori_public_key, "#define OEM_PUBK          " + img_verify_public_key_splited + "\n")
	
	#save keys
	open("tempfile/cust_sec_ctrl.h", "w+").write(cust_sec_ctrl_file)
	
	#if storage type is nand, additional work is need
	if configs['general']['storage'] == "NAND":
		#get m_sec_ro_offset and m_sec_cfg_offset from scatter file
		seccfg_found = 0
		secro_found = 0
		sec_ro_offset = 0
		seccfg_offset = 0
		for line in open(configs['general']['scatter']):
			line = line.lower()
			if line.find("seccfg") != -1 or line.find("sec_cfg") != -1:
				seccfg_found = 1
			
			if line.find("secro") != -1 or line.find("sec_ro") != -1:
				secro_found = 1
			
			if seccfg_found == 1 and line.find("linear_start_addr") != -1:
				seccfg_offset = line.split(":")[1].strip()
				seccfg_found = 0
			
			if secro_found == 1 and line.find("linear_start_addr") != -1:
				sec_ro_offset = line.split(":")[1].strip()
				secro_found = 0
		
		#m_sec_ro_offset and m_sec_cfg_offset should be set in sec_rom_info.c
		sec_rom_info_path = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/%s/src/security/sec_rom_info.c" % (configs['general']['codebase'], configs['general']['platform'])
		sec_rom_info_file = ""
		m_sec_ro_offset_found = 0
		m_sec_cfg_offset_found = 0
		for line in open(sec_rom_info_path):
			if line.find("m_sec_ro_offset") != -1:
				m_sec_ro_offset_found += 1
				if m_sec_ro_offset_found == 2:
					line = line.split("=")[0] + "= " + sec_ro_offset + "\n"
			
			if line.find("m_sec_cfg_offset") != -1:
				m_sec_cfg_offset_found += 1
				if m_sec_cfg_offset_found == 2:
					line = line.split("=")[0] + "= " + seccfg_offset + "\n"
			sec_rom_info_file += line
		
		#save file
		open("tempfile/sec_rom_info.c", "w+").write(sec_rom_info_file)
			
	#generate hardware secureboot related files
	def split_key(ori_key):
		#split daa key
		splited_key = ""
		count = 0
		for ch in ori_key:
			if count % 4 == 0:
				splited_key += "0x"
			splited_key += ch
			if (count+1) % 4 == 0 and (count+1) != len(ori_key):
				splited_key += ","
			count += 1
		return splited_key
		
	#load keys
	if configs['feature']['hardware_secure_ctrl']['Enable_SBC'] == "true":
		
		(sbc_public_key, sbc_private_key) = parse_key(configs['keyfile']['sbc_key'])
		
		#set sbc key in vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/tb8127p4/security/chip_config/s/key/CHIP_TEST_KEY.ini
		chip_test_key_file = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/%s/security/chip_config/s/key/CHIP_TEST_KEY.ini" % (configs['general']['codebase'], configs['general']['project'])
		chip_test_key_file_new = ""
		for line in open(chip_test_key_file):
			if line.find("private_key_d") != -1:
				line = "private_key_d = \"%s\"" % sbc_private_key + "\n"
			
			if line.find("private_key_n") != -1:
				line = "private_key_n = \"%s\"" % sbc_public_key + "\n"
			
			if line.find("public_key_n") != -1:
				line = "public_key_n  = \"%s\"" % sbc_public_key + "\n"
			
			chip_test_key_file_new += line
		
		#save chip_test_key_file_new 
		open("tempfile/CHIP_TEST_KEY.ini", "w+").write(chip_test_key_file_new)
		
	if configs['feature']['hardware_secure_ctrl']['Enable_DAA'] == "true":
		(daa_public_key, daa_private_key) = parse_key(configs['keyfile']['daa_key'])
		#generate SignDA_SV5.ini
		#read SignDA_SV5.ini
		signda_v5_file = open("template/SignDA_SV5.ini").read()
		
		#split daa keys
		daa_public_key_splited = split_key(daa_public_key)
		daa_private_key_splited = split_key(daa_private_key)
		
		#set keys
		signda_v5_file = signda_v5_file.replace("REPLACE_DAA_RSA_N", daa_public_key_splited)
		signda_v5_file = signda_v5_file.replace("REPLACE_DAA_RSA_D", daa_private_key_splited)
		
		#save SignDA_SV5.ini
		open("tempfile/SignDA_SV5.ini", "w+").write(signda_v5_file)

		#check if sla enable, if sla is not enable, generate AuthGen_SV5.ini here
		if configs['feature']['hardware_secure_ctrl']['Enable_SLA'] != "true":
			#generate AuthGen_SV5.ini
			authgen_v5_file = open("template/AuthGen_SV5.ini").read()
			authgen_v5_file = authgen_v5_file.replace("REPLACE_DAA_RSA_N", daa_public_key)
			
			#set sbc key
			if configs['feature']['hardware_secure_ctrl']['Enable_SBC'] == "true":
				authgen_v5_file = authgen_v5_file.replace("REPLACE_SBC_RSA_N", sbc_public_key)
				authgen_v5_file = authgen_v5_file.replace("REPLACE_SBC_RSA_D", sbc_private_key)
			else:
				#sbc is not enable, use default sbc key 
				authgen_v5_file = authgen_v5_file.replace("REPLACE_SBC_RSA_N", default_sbc_public_key_n)
				authgen_v5_file = authgen_v5_file.replace("REPLACE_SBC_RSA_D", default_sbc_private_key_n)
			
			
			#daa key is not enable, use default daa key
			authgen_v5_file = authgen_v5_file.replace("REPLACE_SLA_RSA_N", default_sla_public_key_n)
			
			#save keys
			open("tempfile/AuthGen_SV5.ini", "w+").write(authgen_v5_file)
	
	if configs['feature']['hardware_secure_ctrl']['Enable_SLA'] == "true":
		(sla_public_key, sla_private_key) = parse_key(configs['keyfile']['sla_key'])
	
		#generate AuthGen_SV5.ini
		authgen_v5_file = open("template/AuthGen_SV5.ini").read()
		authgen_v5_file = authgen_v5_file.replace("REPLACE_SLA_RSA_N", sla_public_key)
		
		if configs['feature']['hardware_secure_ctrl']['Enable_SBC'] == "true":
			authgen_v5_file = authgen_v5_file.replace("REPLACE_SBC_RSA_N", sbc_public_key)
			authgen_v5_file = authgen_v5_file.replace("REPLACE_SBC_RSA_D", sbc_private_key)
		else:
			#sbc is not enable, use default sbc key 
			authgen_v5_file = authgen_v5_file.replace("REPLACE_SBC_RSA_N", default_sbc_public_key_n)
			authgen_v5_file = authgen_v5_file.replace("REPLACE_SBC_RSA_D", default_sbc_private_key_n)
		
		if configs['feature']['hardware_secure_ctrl']['Enable_DAA'] == "true":
			authgen_v5_file = authgen_v5_file.replace("REPLACE_DAA_RSA_N", daa_public_key)
		else:
			#daa key is not enable, use default daa key
			authgen_v5_file = authgen_v5_file.replace("REPLACE_DAA_RSA_N", default_daa_public_key_n)
		
		#save keys
		open("tempfile/AuthGen_SV5.ini", "w+").write(authgen_v5_file)
		
#copy file
def modify_code(configs):
	#1.copy tempfile/IMG_AUTH_KEY.ini to vendor/mediatek/proprietary/custom/${project}/security/image_auth/IMG_AUTH_KEY.ini
	img_auth_key_file = "%s/vendor/mediatek/proprietary/custom/%s/security/image_auth/" % (configs['general']['codebase'], configs['general']['project'])
	cmdline = "cp -f %s %s" % ("tempfile/IMG_AUTH_KEY.ini", img_auth_key_file)
	os.system(cmdline)
	print "setting: " + img_auth_key_file + "IMG_AUTH_KEY.ini"
	
	#2.copy tempfile/VERIFIED_BOOT_IMG_AUTH_KEY.ini to 	vendor/mediatek/proprietary/custom/${project}/security/image_auth/
	verified_key_file = "%s/vendor/mediatek/proprietary/custom/%s/security/image_auth/" % (configs['general']['codebase'], configs['general']['project'])
	cmdline = "cp -f %s %s" % ("tempfile/VERIFIED_BOOT_IMG_AUTH_KEY.ini", verified_key_file)
	os.system(cmdline)
	print "setting: " + verified_key_file + "VERIFIED_BOOT_IMG_AUTH_KEY.ini"
	
	#3.copy oemkey.h to vendor/mediatek/proprietary bootable/bootloader/lk/target/${PROJECT}/inc/oemkey.h
	ome_key_file = "%s/vendor/mediatek/proprietary/bootable/bootloader/lk/target/%s/inc/" % (configs['general']['codebase'], configs['general']['project'])
	cmdline = "cp -f %s %s" % ("tempfile/oemkey.h", ome_key_file)
	os.system(cmdline)
	print "setting: " + ome_key_file + "oemkey.h"
	
	#4.copy cust_sec_ctrl.h to vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/${PROJECT}/inc/cust_sec_ctrl.h
	cust_sec_key_file = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/%s/inc/" % (configs['general']['codebase'], configs['general']['project'])
	cmdline = "cp -f %s %s" % ("tempfile/cust_sec_ctrl.h", cust_sec_key_file)
	os.system(cmdline)
	print "setting: " + cust_sec_key_file + "cust_sec_ctrl.h"
	
	#5.set projct mk file
	project_mk_file = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/%s/%s.mk" % (configs['general']['codebase'], configs['general']['project'], configs['general']['project'])
	cmdline = "cp -f %s %s" % ("tempfile/%s.mk" % (configs['general']['project']), project_mk_file)
	os.system(cmdline)
	print "setting: " + project_mk_file
	
	#6.set SECRO_GMP.ini
	secro_file = "%s/vendor/mediatek/proprietary/custom/common/secro/SECRO_GMP.ini" % (configs['general']['codebase'])
	cmdline = "cp -f %s %s" % ("template/SECRO_GMP.ini", secro_file)
	os.system(cmdline)
	print "setting: " + secro_file
	
	#7. if nand, set sec_rom_info.c
	if configs['general']['storage'] == "NAND":
		sec_rom_info_file = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/platform/%s/src/security/sec_rom_info.c" % (configs['general']['codebase'], configs['general']['platform'])
		cmdline = "cp -f %s %s" % ("tempfile/sec_rom_info.c", sec_rom_info_file)
		os.system(cmdline)
		print "setting: " + sec_rom_info_file
	
	#8. copy tempfile/CHIP_TEST_KEY.ini
	if configs['feature']['hardware_secure_ctrl']['Enable_SBC'] == "true":
		chip_test_key_file = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/%s/security/chip_config/s/key/CHIP_TEST_KEY.ini" % (configs['general']['codebase'], configs['general']['project'])
		cmdline = "cp -f %s %s" % ("tempfile/CHIP_TEST_KEY.ini", chip_test_key_file)
		os.system(cmdline)
		print "setting: " + chip_test_key_file
	
	#9. disable brommod if need
	if configs['feature']['software_secure_ctrl']['Disable_BROM_MODE'] == "true":
		gfh_sec_cfg_jtag_on_file = "%s/vendor/mediatek/proprietary/bootable/bootloader/preloader/custom/%s/security/chip_config/s/gfh/GFH_SEC_CFG_JTAG_ON.txt" % (configs['general']['codebase'], configs['general']['project'])
		cmdline = "cp -f template/GFH_SEC_CFG_JTAG_ON.txt %s" % (gfh_sec_cfg_jtag_on_file)
		os.system(cmdline)
		print "setting: " + gfh_sec_cfg_jtag_on_file

def parse_key(keyfile):
	temp_key_file = "%s.temp" % keyfile
	cmdline = "openssl rsa -text -in %s -pubout > %s" % (keyfile, temp_key_file)
	os.system(cmdline)
	#check if keyfile generated success
	if not os.path.exists(temp_key_file):
		print "[error] command line excute failed: %s , please check you linux environment" % cmdline
		exit(-1)
	
	#parse keys
	#read keyfile
	lines = ""
	for line in open(temp_key_file):
		lines += line
	
	#replace \r \next
	lines = lines.replace("\n", "")
	lines = lines.replace(" ", "")
	
	#get public key
	#from modulus to publicExponent
	start_idx = lines.find("modulus:") + len("modulus:")
	end_idx = lines.find("publicExponent")
	public_key = lines[start_idx:end_idx]
	if public_key[:2] == "00":
		public_key = public_key[2:]
	public_key = public_key.replace(":", "")
	#print "public_key :" + public_key
	
	#get private key
	#from privateExponent to prime1
	start_idx = lines.find("privateExponent:") + len("privateExponent:")
	end_idx = lines.find("prime1")
	private_key = lines[start_idx:end_idx]
	if private_key[:2] == "00":
		private_key = private_key[2:]
	private_key = private_key.replace(":", "")
	#print "private_key:" + private_key
	
	return (public_key, private_key)

if __name__ == "__main__":
	if not os.path.exists("keyfiles/"):
		os.mkdir("keyfiles/")
	if not os.path.exists("tempfile/"):
		os.mkdir("tempfile/")
	configs = parse_customization_xml()
	generate_code(configs)
	modify_code(configs)
	