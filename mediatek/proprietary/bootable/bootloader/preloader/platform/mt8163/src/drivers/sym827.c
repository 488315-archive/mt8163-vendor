#include "platform.h"
#include "typedefs.h"
#include "sym827.h"
#include "i2c.h" 

/**********************************************************
  *
  *   [I2C Slave Setting]
  *
  *********************************************************/
#define SYM827_SLAVE_ADDR_WRITE   0xc0
#define SYM827_SLAVE_ADDR_READ    0xc1

#define SYM827_MIN_MV	600000
#define SYM827_STEP_MV	12500


#ifdef I2C_EXT_BUCK_CHANNEL
#define SYM827_BUSNUM I2C_EXT_BUCK_CHANNEL
#else
#define SYM827_BUSNUM 1
#endif
#define SYM827_TEST_NODE

#define SYM827_I2C_ID    1 //I2C1

#define slt_print(fmt, args...)   \
do {									\
    printf(fmt, ##args); \
} while(0)

static struct mt_i2c_t sym827_i2c; 

unsigned int g_vproc_en_gpio_number = 0;
unsigned int g_vproc_vsel_gpio_number = 0;

void ext_buck_vproc_vsel(int val)
{
	sym827_buck_set_switch(val);
}

/**********************************************************
  *
  *   [Global Variable]
  *
  *********************************************************/

int g_sym827_driver_ready = 0;
int g_sym827_hw_exist = 0;
/**********************************************************
  *
  *   [I2C Function For Read/Write sym827]
  *
  *********************************************************/
int sym827_read_byte(kal_uint8 addr, kal_uint8 *dataBuffer)
{
	int ret_code = I2C_OK;
	kal_uint16 len;
	*dataBuffer = addr;

	sym827_i2c.id = SYM827_I2C_ID;
	//sym827_i2c.path_dir = 0;
	/* Since i2c will left shift 1 bit, we need to set sym827 I2C address to >>1 */
	sym827_i2c.addr = (SYM827_SLAVE_ADDR_WRITE >> 1);
	sym827_i2c.mode = ST_MODE;
	sym827_i2c.speed = 100;
	sym827_i2c.pushpull = 1;
	len = 1;
	sym827_i2c.st_rs = I2C_TRANS_REPEATED_START;
	//sym827_i2c.is_rs_enable = 1;

	ret_code = i2c_write_read(&sym827_i2c, dataBuffer, len, len);
	//printf("%s: i2c_read: ret_code: %d\n", __func__, ret_code);

	if(ret_code == 0)
		return 1; // ok
	else
		return 0; // fail
}

int sym827_write_byte(kal_uint8 addr, kal_uint8 value)
{
	int ret_code = I2C_OK;
	kal_uint8 write_data[2];
	kal_uint16 len;

	write_data[0]= addr;
	write_data[1] = value;

	sym827_i2c.id = SYM827_I2C_ID;
	//sym827_i2c.path_dir = 0;
	/* Since i2c will left shift 1 bit, we need to set sym827 I2C address to >>1 */
	sym827_i2c.addr = (SYM827_SLAVE_ADDR_WRITE >> 1);
	sym827_i2c.mode = ST_MODE;
	sym827_i2c.speed = 100;
	sym827_i2c.pushpull = 1;
	sym827_i2c.st_rs = I2C_TRANS_REPEATED_START;
	len = 2;

	ret_code = i2c_write(&sym827_i2c, write_data, len);
	//printf("%s: i2c_write: ret_code: %d\n", __func__, ret_code);

	if(ret_code == 0)
		return 1; // ok
	else
		return 0; // fail
}

/**********************************************************
  *
  *   [Read / Write Function]
  *
  *********************************************************/
kal_uint32 sym827_read_interface(kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	kal_uint8 sym827_reg = 0;
	int ret = 0;

	ret = sym827_read_byte(RegNum, &sym827_reg);
	sym827_reg &= (MASK << SHIFT);
	*val = (sym827_reg >> SHIFT);

	return ret;
}

kal_uint32 sym827_config_interface(kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT)
{
	kal_uint8 sym827_reg = 0;
	int ret = 0;

	ret = sym827_read_byte(RegNum, &sym827_reg);
	sym827_reg &= ~(MASK << SHIFT);
	sym827_reg |= (val << SHIFT);

	ret = sym827_write_byte(RegNum, sym827_reg);

	return ret;
}

kal_uint32 sym827_get_reg_value(kal_uint32 reg)
{
	kal_uint32 ret = 0;
	kal_uint8 reg_val = 0;

	ret = sym827_read_interface((kal_uint8) reg, &reg_val, 0xFF, 0x0);

	return reg_val;
}

/**********************************************************
  *
  *   [Internal Function]
  *
  *********************************************************/
void sym827_dump_register(void)
{
	kal_uint8 i = 0;

	for (i = SYM827_REG_VSEL_0; i <= SYM827_REG_PGOOD; i++)
		slt_print("[sym827] [0x%x]=0x%x\n", i, sym827_get_reg_value(i));

}

int sym827_buck_power_on()
{
	int ret = 0;

	ret = sym827_config_interface(SYM827_REG_VSEL_0,
			SYM827_BUCK_ENABLE, 0x1, SYM827_BUCK_EN0_SHIFT);
	ret = sym827_config_interface(SYM827_REG_VSEL_1,
			SYM827_BUCK_ENABLE, 0x1, SYM827_BUCK_EN0_SHIFT);

	return ret;
}

int sym827_buck_power_off()
{
	int ret = 0;

	if (!is_sym827_sw_ready()) {
		slt_print("sym827 driver is not exist\n");
		return 0;
	}
	if (!is_sym827_exist()) {
		slt_print("sym827 is not exist\n");
		return 0;
	}

	ret = sym827_config_interface(SYM827_REG_VSEL_0,
			SYM827_BUCK_DISABLE, 0x1, SYM827_BUCK_EN0_SHIFT);
	ret = sym827_config_interface(SYM827_REG_VSEL_1,
			SYM827_BUCK_DISABLE, 0x1, SYM827_BUCK_EN0_SHIFT);

	return ret;
}

int sym827_buck_get_state()
{
	int ret = 0;
	kal_uint8 reg = 0;
	kal_uint8 val = 0;

	if (!is_sym827_sw_ready()) {
		slt_print("sym827 driver is not exist\n");
		return 0;
	}
	if (!is_sym827_exist()) {
		slt_print("sym827 is not exist\n");
		return 0;
	}

	ret = sym827_read_interface(SYM827_REG_VSEL_0, &val, 0x1, SYM827_BUCK_EN0_SHIFT);

	return val;
}

void sym827_hw_init(void)
{
	kal_uint32 ret = 0;
	sym827_buck_set_voltage(SYM827_REG_VSEL_0, 1150000);
	sym827_buck_set_voltage(SYM827_REG_VSEL_1, 1150000);
	sym827_buck_power_on();
}

void sym827_hw_component_detect(void)
{
	kal_uint32 ret = 0;
	kal_uint8 val = 0;

	ret = sym827_read_interface(SYM827_REG_ID_1, &val, 0x07, SYM827_ID_VENDOR_SHIFT);

	/* check default SPEC. value */
	if (val == 0x04)
		g_sym827_hw_exist = 1;
	else
		g_sym827_hw_exist = 0;

	slt_print("val = %d\n", val);
}

int is_sym827_sw_ready(void)
{
	return g_sym827_driver_ready;
}

int is_sym827_exist(void)
{
	return g_sym827_hw_exist;
}

int sym827_buck_set_mode(int mode)
{
	int ret = 0;
	kal_uint8 reg = 0;

	if (!is_sym827_sw_ready()) {
		slt_print("sym827 driver is not exist\n");
		return 0;
	}
	if (!is_sym827_exist()) {
		slt_print("sym827 is not exist\n");
		return 0;
	}

	ret = sym827_config_interface(SYM827_REG_VSEL_0,
			mode, 0x1, SYM827_BUCK_MODE_SHIFT);
	ret = sym827_config_interface(SYM827_REG_VSEL_1,
			mode, 0x1, SYM827_BUCK_MODE_SHIFT);

	return ret;
}

int sym827_buck_get_mode()
{
	int ret = 0;
	kal_uint8 reg = 0;
	kal_uint8 val = 0;

	if (!is_sym827_sw_ready()) {
		slt_print("sym827 driver is not exist\n");
		return 0;
	}
	if (!is_sym827_exist()) {
		slt_print("sym827 is not exist\n");
		return 0;
	}

	ret = sym827_read_interface(SYM827_REG_VSEL_0, &val, 0x1, SYM827_BUCK_MODE_SHIFT);
	return val;
}

int sym827_buck_set_switch(int val)
{
	int ret = 0;
	kal_uint8 reg = 0;

	return ret;
}

int sym827_buck_get_switch()
{
	int ret = 0;

	return ret;
}

int sym827_buck_set_voltage(int vout_sel, unsigned long voltage)
{
	int ret = 1;
	int vol_sel = 0;
	kal_uint8 reg = 0;

	/* 600mV~13875mV, step=12.5mV */
	ret = sym827_read_interface(vout_sel, &reg, 0x80, 0x0);
	if (!ret) {
		slt_print("I2C read failed, do noting...\n");
		return ret;
	}

	vol_sel = ((voltage) - SYM827_MIN_MV) / SYM827_STEP_MV;
	if (vol_sel > 63)
		vol_sel = 63;

	ret = sym827_write_byte(vout_sel, vol_sel | reg);

	slt_print("vol_sel = %x, get = %x\n",
		vol_sel, sym827_get_reg_value(vout_sel));

	return ret;
}

unsigned long sym827_buck_get_voltage()
{
	kal_uint8 vol_sel = 0;
	kal_uint8 reg = 0;
	unsigned long voltage = 0;

	sym827_read_interface(SYM827_REG_VSEL_0, &vol_sel,
		0x3F, SYM827_BUCK_NSEL_SHIFT);
	voltage = (SYM827_MIN_MV + vol_sel * SYM827_STEP_MV);
	return voltage;
}

int sym827_init(void)
{
	int err = 0;
	slt_print("[sym827_driver_probe]\n");

	sym827_hw_component_detect();

	if (g_sym827_hw_exist == 1) {
		sym827_hw_init();
		sym827_dump_register();
	}
	g_sym827_driver_ready = 1;

	if (g_sym827_hw_exist == 0) {
		slt_print("[sym827_driver_probe] return err\n");
		return err;
	}

	return 0;
}

