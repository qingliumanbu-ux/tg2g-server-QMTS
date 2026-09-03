/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-05-17 9:13:28
Description: 发送铁区质量数据
调用方式:
bcls_rec->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");  --计划号
bcls_rec->Tables[0].Columns.Add(DT_STRING, "SAMPLE_LOT_NO");      --熔炼号

bcls_rec->Tables[0].Rows.Add();
bcls_rec->Tables[0].Rows[0]["HEAT_NO"] = "H2406225";
bcls_rec->Tables[0].Rows[0]["SAMPLE_LOT_NO"] = "H2406225-F1S-3#-1";
**************************************************/

#include "stdafx.h"
#include "epex.h"


BM2_FUNCTION_EXPORT
int f_qmts_21b004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts24("TQMTS24");
	CModel tmmsm11("TMMSM11");
	CDbCommand cmd(conn);
	EPEX epex;

	try
	{
		tqmts24["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"];
		tqmts24.Query("ST_SAMPLE_NO");
		sqlstr = "SELECT *  FROM TMMSM11 WHERE heat_no  = @heat_no ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", tqmts24["HEAT_NO"]);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			cmd.Fetch(tmmsm11);
			//发21B004电文
			if (epex.Initialize("21B004") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004","DEAL_FLAG", 0, "I") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004", "TCP_NO", 0, tmmsm11["TAPNO"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004", "TPC_SEQ", 0, tmmsm11["TPC_ID"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004", "TPC_NO", 0, tmmsm11["TPC_YL_NO"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
			if (epex.SetValue("21B004", "SAMPLE_NO", 0, tqmts24["ST_SAMPLE_NO"].ToString()) < 0)
			{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SetValue("21B004", "MAT_CODE", 0, "TS0000") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004", "MAT_CNAME", 0, "普通铁水") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004", "SAMPLE_POS_CODE", 0, tqmts24["DEV_CODE"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004", "ANALYSE_TIME", 0, tqmts24["ANALYSE_TIME"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004", "SAMPLE_TIME", 0, tqmts24["SAMPLE_TAKEN_TIME"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004", "ANALYSE_ITEM_NUM", 0, 7) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//循环表
			//0
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_CODE", 0, "Y001") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_NAME", 0, "C") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_DATA_TYPE", 0, "Y") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_VALUE", 0, tqmts24["ELM_001"].ToDecimal()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			// 1
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_CODE", 1, "Y002") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_NAME", 1, "Si") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_DATA_TYPE", 1, "Y") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_VALUE", 1, tqmts24["ELM_002"].ToDecimal()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			// 2
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_CODE", 2, "Y003") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_NAME", 2, "Si") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_DATA_TYPE", 2, "Y") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_VALUE", 2, tqmts24["ELM_003"].ToDecimal()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			// 3
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_CODE", 3, "Y004") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_NAME", 3, "Mn") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_DATA_TYPE", 3, "Y") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_VALUE", 3, tqmts24["ELM_004"].ToDecimal()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			// 4
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_CODE", 4, "Y005") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_NAME", 4, "S") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_DATA_TYPE", 4, "Y") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_VALUE", 4, tqmts24["ELM_005"].ToDecimal()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			// 5
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_CODE", 5, "Y028") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_NAME", 5, "V") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_DATA_TYPE", 5, "Y") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_VALUE", 5, tqmts24["ELM_012"].ToDecimal()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			// 6
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_CODE", 6, "Y006") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_NAME", 6, "Ti") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_DATA_TYPE", 6, "Y") < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("21B004_1", "ANALYSE_ITEM_VALUE", 6, tqmts24["ELM_013"].ToDecimal()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SendTele() < 0)
			{
				Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			epex.Uninitialize();
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


