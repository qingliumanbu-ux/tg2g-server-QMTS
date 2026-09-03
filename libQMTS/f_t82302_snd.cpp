/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: 二炼钢表面电文
**************************************************/ 

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t82302_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString v_value = "";//获取值类型 发送数据 
	CString v_surface_decide_code = "";//根据小代码确认有值，没有则发机组号
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CModel tmmsm01("TMMSM01");
	CDbCommand cmd_inq(conn);
	try
	{
		CString epex_number = "T82302";
		CString deal_flag = "1";
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (bcls_rec->Tables[0].Columns.Contains("T82302"))
			v_value = bcls_rec->Tables[0].Rows[0]["T82302"].ToString().Trim();

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tmmsm01["MAT_NO"] = bcls_rec->Tables[0].Rows[i]["MAT_NO"];
			tmmsm01.Query();

			//47-合格  产出时默认值   不发
			sqlstr = " select CODE_DESC_1_CONTENT from TWMSMZD02 where CODE_CLASS='MMBMZL'  and code = '" + tmmsm01["SURF_QUALITY"].ToString() + "' and code <> '47' ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				v_surface_decide_code = cmd_inq.GetString(1);
			}
			else
			{
				v_surface_decide_code ="";//没查到置空
			}
			cmd_inq.Close();


			if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("SLAB_NO", 0, tmmsm01["SLAB_NO"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("LG_ST", 0, tmmsm01["ST_NO"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("HEAT_NO", 0, tmmsm01["HEAT_NO"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("CUT_TIME", 0, tmmsm01["SLAB_CUT_TIME"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("VALUE_TYPE", 0, v_value) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (v_surface_decide_code != "")//不为空时为过渡坯，发过渡坯
			{
				if (epex.SetValue("VALUE", 0, v_surface_decide_code) < 0)
				{
					sprintf(s.msg, (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			else if (v_surface_decide_code == "")//等于空的时候发机组号
			{
				if (epex.SetValue("VALUE", 0, tmmsm01["UNIT_CODE"].ToString()) < 0)
				{
					sprintf(s.msg, (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			
			if (epex.SetValue("SEND_TIME", 0, s.datetime) < 0)
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
		}
		epex.Uninitialize();
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


