/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      李振
Version:     1.0
Date:        2025-01-14
Description: 消息引擎判定结果
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>

/// </summary>
/// <param name>  </param>
/// <returns>  </returns>
===========================================================</remark>*/

void f_call_ijudge_svc(CDbConnection* conn, const CString& system_code, const CString& envType, EIClass* blks_in, EIClass* blks_out, bool traceJso);
void f_call_ijudge_svc(CDbConnection* conn, const CString& system_code, const CString& envType, const CString& apiCode, EIClass* blks_in, EIClass* blks_out, bool traceInJson, bool traceOutJson);
BM2_FUNCTION_EXPORT
int f_qmts_xxyq(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";
	CString datetime = " ";
	struct ei_sys s_tmp;


	/* 数据库操作类定义：统一放在Service或函数前段 */
	CModel twmsmxxyq("TWMSMXXYQ");
	CDbCommand cmd_inq(conn);

	try
	{
		Log::Trace(s.svc_name, "", "-----------f_qmts_xxyq begin------------");
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", "", "datetime=[{0}]", datetime);
		CString system_code = "TGT8Z_TASK";
		CString envType = "0";  //0-模拟；1-正式
		CString apiCode = "GetCode";
		try
		{
			twmsmxxyq.Reset();
			twmsmxxyq["REC_CREATE_TIME"] = datetime;
			twmsmxxyq["REC_CREATOR"] = s.userid;
			twmsmxxyq["TIME_STAMPS"] = datetime;

			f_call_ijudge_svc(conn, system_code, envType, bcls_rec, bcls_ret, true);
			bcls_ret->GetSYS(&s_tmp);
			if (s_tmp.flag < 0)
			{
				//处理引擎接口本身处理逻辑异常
				throw CApplicationException(CString::Format("s.flag=%d s.msg=%s s.sysmsg=%s", s_tmp.flag, s_tmp.msg, s_tmp.sysmsg));
			}
			else
			{
				Log::Trace("", "", "Call successfully!");
				Log::Trace("", "", "Call successfully!", bcls_ret->Tables.get_Count());

				if (bcls_ret->Tables["RULE_INFO"].Rows.get_Count() > 0)
				{
					twmsmxxyq["CODE"] = bcls_ret->Tables["RULE_INFO"].Rows[0]["RULE_CODE"].ToString();
					twmsmxxyq["CODE_DESC"] = bcls_ret->Tables["RULE_INFO"].Rows[0]["RULE_DESC"].ToString();
				}
				if (bcls_ret->Tables["PROJECT_CONFIG"].Rows.get_Count() > 0)
				{
					twmsmxxyq["HEAT_NO"] = bcls_ret->Tables["PROJECT_CONFIG"].Rows[0]["HEAT_NO"].ToString();
					twmsmxxyq["DEV_CODE"] = bcls_ret->Tables["PROJECT_CONFIG"].Rows[0]["DEV_CODE"].ToString();
					twmsmxxyq["ST_NO"] = bcls_ret->Tables["PROJECT_CONFIG"].Rows[0]["ST_NO"].ToString();
				}
				if (bcls_ret->Tables["DIRECT_INFO"].Rows.get_Count() > 0)
				{
					Log::Trace("", "", "LINE", bcls_ret->Tables["DIRECT_INFO"].Rows.get_Count());
					twmsmxxyq["REMARK"] = bcls_ret->Tables["DIRECT_INFO"].Rows[0]["MSG"].ToString();
				}
				if (bcls_ret->Tables.Contains("GROUP_TYPE_MAPPING") && bcls_ret->Tables["GROUP_TYPE_MAPPING"].Rows.get_Count() > 0)
				{
					twmsmxxyq["INFO_CODE"] = bcls_ret->Tables["GROUP_TYPE_MAPPING"].Rows[0]["GROUP_TYPE"].ToString();

				}
			}
			f_call_ijudge_svc(conn, system_code, envType, apiCode, bcls_rec, bcls_ret, true, true);
			if (s_tmp.flag < 0)
			{
				//处理引擎接口本身处理逻辑异常
				throw CApplicationException(CString::Format("s.flag=%d s.msg=%s s.sysmsg=%s", s_tmp.flag, s_tmp.msg, s_tmp.sysmsg));
			}
			else
			{
				Log::Trace("", "", "Call successfully!");
				Log::Trace("", "", "Call successfully!", bcls_ret->Tables[0].get_TableName());
				if (bcls_ret->Tables.Contains("EPIJG0") && bcls_ret->Tables["EPIJG0"].Rows.get_Count() > 0 && twmsmxxyq["INFO_CODE"].ToString().Trim() != "")
				{
					for (int i = 0; i < bcls_ret->Tables["EPIJG0"].Rows.get_Count(); i++)
					{
						if (twmsmxxyq["INFO_CODE"].ToString() == bcls_ret->Tables["EPIJG0"].Rows[i]["CODE"].ToString())
						{
							twmsmxxyq["INFO_TYPE"] = bcls_ret->Tables["EPIJG0"].Rows[i]["CODE_DESC_2_CONTENT"].ToString();
							break;
						}
					}


				}

			}
			Log::Trace("", "", "111=[{0}]", twmsmxxyq["CODE"].ToString().Trim());
			Log::Trace("", "", "222=[{0}]", twmsmxxyq["REMARK"].ToString().Trim());

			if (twmsmxxyq["CODE"].ToString().Trim() != "")
			{
				Log::Trace("", "", "LINE", 1);
				twmsmxxyq.Print();
				twmsmxxyq.Insert();
			}
		}
		catch (const CApplicationException& ex)
		{
			//处理引擎接口调用异常
			throw CApplicationException(CString::Format("Call RestService [%s] failed.%s", (const char*)system_code, (const char*)ex.GetMsg()));
		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}

