/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-11-7 15:08:16
Description: 成分标准（北）
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//发送专家系统数据

BM2F_ENTERACE_TELE(qm_002112_rcv)

int f_qm_002112_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);

	CModel tqmts02("TQMTS02");
	CModel tqmts0x("TQMTS0X");
	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82313";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tqmts0x);
	in_23m.Tables.Add();
	in_23m.Tables[2].Columns.Add(tqmts02);
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		for (int i = 0; i < bcls_rec->Tables["tqmtms0_h"].Rows.get_Count(); i++)
		{
			tqmts02["PROC_DIV"] = bcls_rec->Tables["tqmtms0_h"].Rows[i]["proc_div"].ToString();
			tqmts02["IDX_NO"] = bcls_rec->Tables["tqmtms0_h"].Rows[i]["idx_no"].ToString();
			tqmts02.Delete("IDX_NO");

			sqlstr = " select * from tqmts0x where ELM_STD_IDX_A='" + tqmts02["IDX_NO"].ToString() + "' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			while (cmd_inq.Read())
			{
				cmd_inq.Fetch(tqmts0x);
				tqmts0x.MergeTo(in_23m.Tables[1]);
			}
			cmd_inq.Close();

			for (int i = 0; i < bcls_rec->Tables["tqmtms0"].Rows.get_Count(); i++)
			{
				tqmts02["TABLE_ITEM_SEQ"] = bcls_rec->Tables["tqmtms0"].Rows[i]["seq_no"].ToString();

				tqmts02["ELM_CODE"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_code"].ToString();
				tqmts02["ELM_NAME"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_name"].ToString();
				tqmts02["ELM_MAIN_AIM_TYPE"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_aim_type"].ToString();
				tqmts02["MAIN_AIM"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_aim"].ToString();
				tqmts02["ELM_MAIN_MIN_TYPE"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_min_type"].ToString();

				tqmts02["MAIN_MIN"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_min"];

				tqmts02["ELM_MAIN_MIN_FORMULAR"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_min_formular"].ToString();

				tqmts02["ELM_MAIN_MIN_FORMULAR_DESC"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_min_formular_desc"].ToString();

				tqmts02["ELM_MAIN_MAX_TYPE"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_max_type"].ToString();

				tqmts02["MAIN_MAX"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_max"].ToString();

				tqmts02["ELM_MAIN_MAX_FORMULAR"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_max_formular"].ToString();

				tqmts02["ELM_MAIN_MAX_FORMULAR_DESC"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_main_max_formular_desc"].ToString();

				tqmts02["ELM_SPE_MIN_TYPE"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_spe_min_type"].ToString();

				tqmts02["SPE_MIN"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_spe_min"].ToString();

				tqmts02["ELM_SPE_MIN_FORMULAR"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_spe_min_formular"].ToString();

				tqmts02["ELM_SPE_MIN_FORMULAR_DESC"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_spe_min_formular_desc"].ToString();

				tqmts02["ELM_SPE_MAX_TYPE"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_spe_max_type"].ToString();

				tqmts02["SPE_MAX"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_spe_max"].ToString();

				tqmts02["ELM_SPE_MAX_FORMULAR"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_spe_max_formular"].ToString();

				tqmts02["ELM_SPE_MAX_FORMULAR_DESC"] = bcls_rec->Tables["tqmtms0"].Rows[i]["elm_spe_max_formular_desc"].ToString();

				tqmts02["REMARK"] = bcls_rec->Tables["tqmtms0"].Rows[i]["remark"].ToString();

				tqmts02["REC_CREATOR"] = s.userid;
				tqmts02["REC_CREATE_TIME"] = datetime;
				tqmts02.Insert();

				tqmts02.MergeTo(in_23m.Tables[2]);
			
			}
		}

		if (in_23m.Tables[1].Rows.get_Count() > 0)
		{
			doFlag = f_t8z_23m_snd(&in_23m, bcls_ret, conn);
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


