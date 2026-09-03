/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-11-23 15:08:16
Description: 工艺卡接收
**************************************************/

#include "stdafx.h"
#include "epex.h"

/* ***** 外部函数申明 ***** */
int f_mmsm33czxs_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //工艺卡有新钢种(调用物料函数)
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//工艺卡钢种下发 发送宝武聊天

BM2F_ENTERACE_TELE(qm_002110_rcv)


int f_qm_002110_rcv(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmts0x("TQMTS0X");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "ST_NO");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "C_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");

	try
	{
		tqmts0x.MergeFrom(bcls_rec->Tables["tqmts0x"].Rows[0]);
		tqmts0x["REC_CREATOR"] = s.userid;
		tqmts0x["REC_CREATE_TIME"] = datetime;
		tqmts0x.Delete("ST_NO");
		tqmts0x.Insert();

		//新增NCX_ST_STEEL_CARD_R 表，为了自动化系统的报表（微笑）
		sqlstr = "DELETE  FROM NCX_ST_STEEL_CARD_R WHERE st_no = @st_no";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("st_no", tqmts0x["ST_NO"]);
		cmd.ExecuteNonQuery();

		sqlstr = " insert into NCX_ST_STEEL_CARD_R"
			"			(proc_div,"
			"			st_no,"
			"			st_no_desc,"
			"			st_no_big_class,"
			"			st_no_mid_class,"
			"			st_no_small_class,"
			"			st_no_seq,"
			"			c_div,"
			"			sg_class_seq,"
			"			finance_sg_code,"
			"			valid_flag,"
			"			check_time,"
			"			check_maker,"
			"			steel_group,"
			"			elm_std_idx,"
			"			elm_std_idx_a,"
			"			elm_std_idx_b,"
			"			elm_std_idx_c,"
			"			hot_send_div,"
			"			hdr_dicide,"
			"			flu_time_max,"
			"			slab_surf_check_flag,"
			"			grind_slab_surf_check_flag,"
			"			grind_slab_spec_check_flag,"
			"			cool_charge_temp_min,"
			"			cool_keep_temp_time,"
			"			cool_discharge_temp,"
			"			nz_check_frqn,"
			"			center_sgrg_a,"
			"			center_sgrg_b,"
			"			center_sgrg_c,"
			"			crack_center,"
			"			inter_crack_grade,"
			"			segr,"
			"			macro_crack_internal,"
			"			crack_tri,"
			"			crack_angle,"
			"			inclu,"
			"			macula,"
			"			wafer,"
			"			hore,"
			"			negsand_minus,"
			"			label1,"
			"			label1_l,"
			"			label1_u,"
			"			label2,"
			"			label2_l,"
			"			label2_u,"
			"			label3,"
			"			label3_l,"
			"			label3_u,"
			"			label4,"
			"			label4_l,"
			"			label4_u,"
			"			label5,"
			"			label5_l,"
			"			label5_u,"
			"			label6,"
			"			label7,"
			"			label8,"
			"			label9,"
			"			label10,"
			"			label11,"
			"			label12,"
			"			remark,"
			"			sg_grade_1,"
			"			TIMESTAMP"
			"			)"
			"			select"
			"			proc_div,"
			"			st_no,"
			"			st_no_desc,"
			"			st_no_big_class,"
			"			st_no_mid_class,"
			"			st_no_small_class,"
			"			st_no_seq,"
			"			c_div,"
			"			sg_class_seq,"
			"			finance_sg_code,"
			"			valid_flag,"
			"			check_time,"
			"			check_maker,"
			"			steel_group,"
			"			elm_std_idx,"
			"			elm_std_idx_a,"
			"			elm_std_idx_b,"
			"			elm_std_idx_c,"
			"			hot_send_div,"
			"			hdr_dicide,"
			"			flu_time_max,"
			"			slab_surf_check_flag,"
			"			grind_slab_surf_check_flag,"
			"			grind_slab_spec_check_flag,"
			"			cool_charge_temp_min,"
			"			cool_keep_temp_time,"
			"			cool_discharge_temp,"
			"			nz_check_frqn,"
			"			center_sgrg_a,"
			"			center_sgrg_b,"
			"			center_sgrg_c,"
			"			crack_center,"
			"			inter_crack_grade,"
			"			segr,"
			"			macro_crack_internal,"
			"			crack_tri,"
			"			crack_angle,"
			"			inclu,"
			"			macula,"
			"			wafer,"
			"			hore,"
			"			negsand_minus,"
			"			label1,"
			"			label1_l,"
			"			label1_u,"
			"			label2,"
			"			label2_l,"
			"			label2_u,"
			"			label3,"
			"			label3_l,"
			"			label3_u,"
			"			label4,"
			"			label4_l,"
			"			label4_u,"
			"			label5,"
			"			label5_l,"
			"			label5_u,"
			"			label6,"
			"			label7,"
			"			label8,"
			"			label9,"
			"			label10,"
			"			label11,"
			"			label12,"
			"			remark,"
			"			sg_grade_1,"
			"			TO_DATE(rec_create_time, 'yyyy-MM-dd HH24@mi@ss')"
			"			from tqmts0x WHERE st_no = @st_no ";

		//工艺卡有新钢种(调用物料函数)
		/*bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["ST_NO"] = tqmts0x["ST_NO"];
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["C_DIV"] = tqmts0x["C_DIV"];
		doFlag = f_mmsm33czxs_proc(&bcls_rec_s, bcls_ret, conn);
		if (doFlag != 0)
		{
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("st_no", tqmts0x["ST_NO"]);
		cmd.ExecuteNonQuery();
		//工艺卡有新钢种 发送宝武聊天
		bcls_rec_s.Tables[0].Rows.Add();
		bcls_rec_s.Tables[0].Rows[0]["ST_NO"] = tqmts0x["ST_NO"];
		bcls_rec_s.Tables[0].Rows[0]["CODE"] = "1";
		bcls_rec_s.Tables[0].Rows[0]["REMARK"] = tqmts0x["ST_NO"];
		Log::Trace("", "", "ST_NO={0}", tqmts0x["ST_NO"].ToString());
		doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
		if (doFlag < 0)
		{
			strcpy(s.msg, "调用函数报错!");
			throw CApplicationException(-1, s.msg, log.Location);
		}
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
