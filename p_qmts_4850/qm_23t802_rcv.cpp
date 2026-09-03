/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-12-11 15:08:16
Description: 二炼钢铸坯分级判定结果
**************************************************/

#include "stdafx.h"
#include "epex.h"

int f_wmsm_t80ryd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
BM2F_ENTERACE_TELE(qm_23t802_rcv)

int f_qm_23t802_rcv(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmtst802("TQMTST802");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	if (bcls_rec->Tables.IndexOf("MM0099") < 0)
	{
		bcls_rec->Tables.Add("MM0099");
		bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		bcls_rec->Tables["MM0099"].Rows.Clear();
	}
	try
	{
		tqmtst802["SLAB_NO"] = bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString();
		tqmtst802["ST_CLASS"] = bcls_rec->Tables[0].Rows[0]["LG_ST"].ToString();
		tqmtst802["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		tqmtst802["CUT_TIME"] = bcls_rec->Tables[0].Rows[0]["CUT_TIME"].ToString();
		tqmtst802["DECIDE_CODE"] = bcls_rec->Tables[0].Rows[0]["PROCESS_JUDGE_RESULT"].ToString();
		tqmtst802["RESP"] = bcls_rec->Tables[0].Rows[0]["PERSON_IN_CHARGE"].ToString();
		tqmtst802["SEND_TIME"] = bcls_rec->Tables[0].Rows[0]["SEND_TIME"].ToString();
		tqmtst802["REMARK"] = bcls_rec->Tables[0].Rows[0]["remark1"].ToString();
		tqmtst802["SAP_ERP_MARK_2"] = bcls_rec->Tables[0].Rows[0]["remark2"].ToString();
		tqmtst802["SAP_ERP_MARK_3"] = bcls_rec->Tables[0].Rows[0]["remark3"].ToString();
		tqmtst802["SAP_ERP_MARK_4"] = bcls_rec->Tables[0].Rows[0]["remark4"].ToString();
		tqmtst802["SAP_ERP_MARK_5"] = bcls_rec->Tables[0].Rows[0]["remark5"].ToString();
		tqmtst802["SAP_ERP_MARK_6"] = bcls_rec->Tables[0].Rows[0]["remark6"].ToString();
		tqmtst802["SAP_ERP_MARK_7"] = bcls_rec->Tables[0].Rows[0]["remark7"].ToString();
		tqmtst802["REMARK_T8"] = bcls_rec->Tables[0].Rows[0]["remark8"];
		tqmtst802["REMARK_T9"] = bcls_rec->Tables[0].Rows[0]["remark9"];
		tqmtst802["REMARK_T10"] = bcls_rec->Tables[0].Rows[0]["remark10"];
		tqmtst802.TrimOrBlank();
		tqmtst802.Delete("HEAT_NO,SLAB_NO");
		tqmtst802.Insert();

		tmmsm01["SLAB_NO"]= bcls_rec->Tables[0].Rows[0]["SLAB_NO"].ToString();
		if (tmmsm01.QueryCount("SLAB_NO") == 1)
		{
			tmmsm01.Query("SLAB_NO");
			EIClass INS;
			INS.Tables[0].Columns.Add(DT_STRING, "SLAB_NO");
			INS.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
			INS.Tables[0].Rows.Add();
			INS.Tables[0].Rows[0]["SLAB_NO"] = tmmsm01["SLAB_NO"];
			INS.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
			doFlag = f_wmsm_t80ryd(&INS, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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
