/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description:  处置F3 F4 F7
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(qmts30_pro)

int f_qmts30_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;
	CString procDiv = "";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	/* 业务变量 */
	CModel tqmts30("TQMTS30");
	CString v_operate = "";
	CString res_process = "";
	CString ck_remark = "";
	CString reason = "";
	CString final_opinion = "";
	CString	steal_remark = "";
	CString	user_name = "";
	CString	v_heat_no_com = "";
	CString	v_st_no_com = "";
	CString	v_change_st_no = "";
	CString	v_to_dev_code = "";
	/* 实体类定义 */

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */

	try
	{
		if (bcls_rec->Tables[1].Columns.Contains("PRO_DIV"))
		{
			v_operate = bcls_rec->Tables[1].Rows[0]["PRO_DIV"].ToString().Trim();
			Log::Trace("", "", "条件查询v_operate[{0}],", v_operate);
		}
		if (bcls_rec->Tables[1].Columns.Contains("RES_PROCESS"))
		{
			res_process = bcls_rec->Tables[1].Rows[0]["RES_PROCESS"].ToString().Trim();
			Log::Trace("", "", "条件查询res_process[{0}],", res_process);
		}
		if (bcls_rec->Tables[1].Columns.Contains("CK_REMARK"))
		{
			ck_remark = bcls_rec->Tables[1].Rows[0]["CK_REMARK"].ToString().Trim();
			Log::Trace("", "", "条件查询ck_remark[{0}],", ck_remark);
		}
		if (bcls_rec->Tables[1].Columns.Contains("FINAL_OPINION"))
		{
			final_opinion = bcls_rec->Tables[1].Rows[0]["FINAL_OPINION"].ToString().Trim();
			Log::Trace("", "", "条件查询final_opinion[{0}],", final_opinion);
		}
		if (bcls_rec->Tables[1].Columns.Contains("STEAL_REMARK"))
		{
			steal_remark = bcls_rec->Tables[1].Rows[0]["STEAL_REMARK"].ToString().Trim();
			Log::Trace("", "", "条件查询steal_remark[{0}],", steal_remark);
		}
		if (bcls_rec->Tables[1].Columns.Contains("USER_NAME"))
		{
			user_name= bcls_rec->Tables[1].Rows[0]["USER_NAME"].ToString().Trim();
			Log::Trace("", "", "条件查询user_name[{0}],", user_name);
		}
		if (bcls_rec->Tables[1].Columns.Contains("REASON"))
		{
			reason= bcls_rec->Tables[1].Rows[0]["REASON"].ToString().Trim();
			Log::Trace("", "", "条件查询reason[{0}],", reason);
		}

		//2025.11.7 增加大炉号炉号,大炉号钢种,改判钢种,改判去向
		if (bcls_rec->Tables[1].Columns.Contains("HEAT_NO_COM"))
		{
			v_heat_no_com = bcls_rec->Tables[1].Rows[0]["HEAT_NO_COM"].ToString().Trim().ToUpper();
			Log::Trace("", "", "条件查询v_heat_no_com[{0}],", v_heat_no_com);
		}
		if (bcls_rec->Tables[1].Columns.Contains("ST_NO_COM"))
		{
			v_st_no_com = bcls_rec->Tables[1].Rows[0]["ST_NO_COM"].ToString().Trim().ToUpper();
			Log::Trace("", "", "条件查询v_st_no_com[{0}],", v_st_no_com);
		}
		if (bcls_rec->Tables[1].Columns.Contains("CHANGE_ST_NO"))
		{
			v_change_st_no = bcls_rec->Tables[1].Rows[0]["CHANGE_ST_NO"].ToString().Trim().ToUpper();
			Log::Trace("", "", "条件查询v_change_st_no[{0}],", v_change_st_no);
		}
		if (bcls_rec->Tables[1].Columns.Contains("TO_DEV_CODE"))
		{
			v_to_dev_code = bcls_rec->Tables[1].Rows[0]["TO_DEV_CODE"].ToString().Trim();
			Log::Trace("", "", "条件查询v_to_dev_code[{0}],", v_to_dev_code);
		}

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tqmts30.Reset();
			tqmts30.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			 if(v_operate == "UPD")
			{
				tqmts30["REC_REVISOR"] = s.userid;
				tqmts30["REC_REVISE_TIME"] = datetime;
				tqmts30["DECIDE_MAKER"] = s.username;
				tqmts30["JUDGE_TIME"] = datetime;
				tqmts30["DECIDER"] = s.username;
				tqmts30["RES_PROCESS"] = res_process;
				tqmts30["CK_REMARK"] = ck_remark;

				tqmts30["HEAT_NO_COM"] = v_heat_no_com;
				tqmts30["ST_NO_COM"] = v_st_no_com;
				tqmts30["CHANGE_ST_NO"] = v_change_st_no;
				tqmts30["TO_DEV_CODE"] = v_to_dev_code;
				
				tqmts30["STATUS_FLAG"] = "1";//待处置

				//判断是否整炉
				if (bcls_rec->Tables[0].Rows[i]["AREA"] = "南区")
				{
					if (bcls_rec->Tables[0].Rows[i]["MAT_THEORY_WT"].ToDecimal() > 50)
					{
						tqmts30["FULL_FURNACE"] = "是";
					}
					else
					{
						tqmts30["FULL_FURNACE"] = "否";
					}

				}
				if (bcls_rec->Tables[0].Rows[i]["AREA"] = "北区")
				{
					if (bcls_rec->Tables[0].Rows[i]["MAT_THEORY_WT"].ToDecimal() > 120)
					{
						tqmts30["FULL_FURNACE"] = "是";
					}
					else
					{
						tqmts30["FULL_FURNACE"] = "否";
					}

				}

				tqmts30.TrimOrBlank();
				tqmts30.Update("REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME,RES_PROCESS,DECIDE_MAKER,CK_REMARK,JUDGE_TIME,DECIDER,STATUS_FLAG,FULL_FURNACE,HEAT_NO_COM,ST_NO_COM,CHANGE_ST_NO,TO_DEV_CODE", "HEAT_NO,ST_NO,MAT_NO");
			}
			else if (v_operate == "USH")
			{
				tqmts30["REC_REVISOR"] = s.userid;
				tqmts30["REC_REVISE_TIME"] = datetime;
				tqmts30["APPROVE_MAKER"] = s.username;
				tqmts30["APPROVE_TIME"] = datetime;
				tqmts30["STATUS_FLAG"] = "3";//最终处置
				tqmts30["FINAL_OPINION"] = final_opinion;
				tqmts30.TrimOrBlank();
				tqmts30.Update("REC_REVISOR,REC_REVISE_TIME,APPROVE_MAKER,APPROVE_TIME,STATUS_FLAG,FINAL_OPINION", "HEAT_NO,ST_NO,MAT_NO");
			}
			else if (v_operate == "BJ")
			{
				tqmts30["STEAL_REMARK"] = steal_remark;
				tqmts30["USER_NAME"] = user_name;
				tqmts30["REASON"] = reason;
				tqmts30.TrimOrBlank();
				tqmts30.Update("STEAL_REMARK,USER_NAME,REASON", "HEAT_NO,ST_NO,MAT_NO");
			}

		}



	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
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
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


