/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-01-15
Description: 成分不合F3 F4 F7
**************************************************/
//框架头文件
#include "stdafx.h"



//业务头文件


//外部函数声明


BM2F_ENTERACE(qmts30lc_pro)

int f_qmts30lc_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
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
	CString message_list = "";
	CString res_process = "";
	CString rejudge_steel = "";
	CString cir_component = "";
	CString note = "";
	CString final_opinion = "";
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
		if (bcls_rec->Tables[1].Columns.Contains("REJUDGE_STEEL"))
		{
			rejudge_steel = bcls_rec->Tables[1].Rows[0]["REJUDGE_STEEL"].ToString().Trim();
			Log::Trace("", "", "条件查询rejudge_steel[{0}],", rejudge_steel);
		}
		if (bcls_rec->Tables[1].Columns.Contains("CIR_COMPONENT"))
		{
			cir_component = bcls_rec->Tables[1].Rows[0]["CIR_COMPONENT"].ToString().Trim();
			Log::Trace("", "", "条件查询cir_component[{0}],", cir_component);
		}

		if (bcls_rec->Tables[1].Columns.Contains("MESSAGE_LIST"))
		{
			message_list = bcls_rec->Tables[1].Rows[0]["MESSAGE_LIST"].ToString().Trim();
			Log::Trace("", "", "条件查询message_list[{0}],", message_list);
		}
		if (bcls_rec->Tables[1].Columns.Contains("FINAL_OPINION"))
		{
			final_opinion = bcls_rec->Tables[1].Rows[0]["FINAL_OPINION"].ToString().Trim();
			Log::Trace("", "", "条件查询final_opinion[{0}],", final_opinion);
		}
		if (bcls_rec->Tables[1].Columns.Contains("NOTE"))
		{
			note = bcls_rec->Tables[1].Rows[0]["NOTE"].ToString().Trim();
			Log::Trace("", "", "条件查询note[{0}],", note);
		}
		
			
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//每次循环先将数据清空 在merge数据
			tqmts30.Reset();
			tqmts30.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			if (v_operate == "UPD")
			{
				tqmts30["REC_REVISOR"] = s.userid;
				tqmts30["REC_REVISE_TIME"] = datetime;
				tqmts30["DECIDE_MAKER"] = s.username;
				tqmts30["JUDGE_TIME"] = datetime;
				tqmts30["STATUS_FLAG"] = "1";//判定状态
				tqmts30["MESSAGE_LIST"] = message_list;
				tqmts30["CIR_COMPONENT"] = cir_component;
				tqmts30["REJUDGE_STEEL"] = rejudge_steel;
				tqmts30["RES_PROCESS"] = res_process;

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
				tqmts30.Update("REC_CREATOR,REC_CREATE_TIME,REC_REVISOR,REC_REVISE_TIME,REJUDGE_STEEL,RES_PROCESS,DECIDE_MAKER,CIR_COMPONENT,CLASSIFY,MESSAGE_LIST,JUDGE_TIME,STATUS_FLAG,FULL_FURNACE", "HEAT_NO,ST_NO,MAT_NO");
			}
			else if (v_operate == "USH")
			{
				tqmts30["REC_REVISOR"] = s.userid;
				tqmts30["REC_REVISE_TIME"] = datetime;
				tqmts30["APPROVE_MAKER"] = s.username;
				tqmts30["STATUS_FLAG"] = "2";//审核状态
				tqmts30["JUDGE_TIME"] = datetime;
				tqmts30["APPROVE_TIME"] = datetime;
				tqmts30.TrimOrBlank();
				tqmts30.Update("REC_REVISOR,REC_REVISE_TIME,APPROVE_MAKER,STATUS_FLAG,JUDGE_TIME,APPROVE_TIME", "HEAT_NO,ST_NO,MAT_NO");
			}
			else if (v_operate == "BH")
			{
				Log::Trace("", "", "条件查询REMARK_4[{0}],", bcls_rec->Tables[0].Rows[i]["REMARK_4"].ToString().Trim());
				Log::Trace("", "", "条件查询REMARK_5[{0}],", bcls_rec->Tables[0].Rows[i]["REMARK_5"].ToString().Trim());
				Log::Trace("", "", "条件查询REMARK_6[{0}],", bcls_rec->Tables[0].Rows[i]["REMARK_6"].ToString().Trim());
				Log::Trace("", "", "条件查询STATUS_FLAG[{0}],", bcls_rec->Tables[0].Rows[i]["STATUS_FLAG"].ToString().Trim());
				Log::Trace("", "", "条件查询final_opinion[{0}],", final_opinion);
				if (bcls_rec->Tables[0].Rows[i]["REMARK_4"].ToString().Trim() == "")
				{
					tqmts30["REMARK_4"] = final_opinion;
					tqmts30["STATUS_FLAG"] = "-1";//驳回1
				}
				else if (bcls_rec->Tables[0].Rows[i]["STATUS_FLAG"].ToString().Trim() == "-1")
				{
					tqmts30["REMARK_5"] = final_opinion;
					tqmts30["STATUS_FLAG"] = "-2";//驳回2
				}
				else if (bcls_rec->Tables[0].Rows[i]["STATUS_FLAG"].ToString().Trim() == "-2")
				{
					tqmts30["REMARK_6"] = final_opinion;
					tqmts30["STATUS_FLAG"] = "-3";//驳回3
				}
				tqmts30["FINAL_OPINION"] = final_opinion;

				tqmts30.TrimOrBlank();
				tqmts30.Update("REMARK_4,STATUS_FLAG,REMARK_5,REMARK_6,FINAL_OPINION", "HEAT_NO,ST_NO,MAT_NO");
			}
			else if (v_operate == "PY")
			{
				tqmts30["NOTE"] = note;

				tqmts30.TrimOrBlank();
				tqmts30.Update("NOTE", "HEAT_NO,ST_NO,MAT_NO");
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


