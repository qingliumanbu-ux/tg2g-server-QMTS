/**************************************************8
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-11-29 11:08:16
Description: 质量判定数据接收
**************************************************/
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(qm_002104_rcv)
int f_mmsm99(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//物料修改函数
int f_mmsm33dbsx_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_mmsm39_proc(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
int f_wmsm_e2t8m1_miss(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//给板坯库二级发取消物料电文
int f_wmsm_t8p302_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);
/*3001-综判合格，3030-综判取消，3013-封锁,3005-判废，3033-判废取消，3006-改判，3007-返修*/

int f_qm_002104_rcv(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");
	CModel tmmsm96("TMMSM96");
	CModel tmmsm33dbsx("TMMSM33DBSX");
	CModel tmmsm34("TMMSM34");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CString ud_time = " ";

	EIClass tmm39;
	tmm39.Tables[0].set_TableName("TMMSM39");
	tmm39.Tables["TMMSM39"].Columns.Add(tmmsm96);
	tmm39.Tables["TMMSM39"].Rows.Clear();
	int row_count = 0;

	EIClass miss_E2T8M1;
	miss_E2T8M1.Tables[0].set_TableName("E2T8M1");
	miss_E2T8M1.Tables["E2T8M1"].Columns.Add(DT_STRING, "MAT_NO");
	miss_E2T8M1.Tables["E2T8M1"].Rows.Clear();

	EIClass inblock;
	inblock.Tables[0].Columns.Add(tmmsm01);
	inblock.Tables[0].Rows.Clear();

	EIClass tmp;
	tmp.Tables[0].Columns.Add(DT_STRING, "MAT_NO");
	tmp.Tables[0].Rows.Clear();
	

	try
	{
		Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
		if (bcls_rec->Tables.IndexOf("MM0099")<0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Rows.Clear();
		}
		Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
		if (bcls_rec->Tables["ZCHO_QM_UD"].Rows.get_Count() == 0)
		{
			sprintf(s.msg, "信息数为空，请联系系统服务人员！");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
		for (int i = 0; i < bcls_rec->Tables["ZCHO_QM_UD"].Rows.get_Count(); i++)
		{
			tmmsm01["MAT_NO"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["BATCH"].ToString();
			hmmsm01["MAT_NO"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["BATCH"].ToString();
			tmmsm01["BATCH"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["BATCH"].ToString();
			hmmsm01["BATCH"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["BATCH"].ToString();
			if (tmmsm01.Query("MAT_NO") || tmmsm01.Query("BATCH"))
			{
				Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
				if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim() == "")
				{
					ud_time = datetime;
				}
				else {
					ud_time = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(0, 4)
						+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(5, 2)
						+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(8, 2)
						+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(11, 2)
						+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(14, 2)
						+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(17, 2);
				}
				tmmsm96.CopyFrom(tmmsm01);
				tmmsm96["UD_TIME"] = ud_time;

				if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3001")//综判合格
				{
					tmmsm96["EVENT_ID"] = "QM61";
					tmmsm96["COMPLEX_DECIDE_CODE"] = "1";
					tmmsm96["USAGE_DECISION"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString();


					
					tmp.Tables[0].Rows.Add();
					tmp.Tables[0].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];
					
				}
				else if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3030")//综判取消
				{
					tmmsm96["EVENT_ID"] = "QM63";

					tmmsm96["USAGE_DECISION"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString();
				}
				else if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3013")//封锁
				{
					tmmsm96["EVENT_ID"] = "QM17";

					tmmsm96["USAGE_DECISION"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString();

				}
				else if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3000")//封锁
				{
					tmmsm96["EVENT_ID"] = "MM08";

					tmmsm96["USAGE_DECISION"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString();

				}
				else if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3005")//判废
				{
					Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
					tmmsm96["EVENT_ID"] = "QM05";

					tmmsm96["USAGE_DECISION"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString();

					miss_E2T8M1.Tables["E2T8M1"].Rows.Add();
					miss_E2T8M1.Tables["E2T8M1"].Rows[i]["MAT_NO"] = tmmsm01["MAT_NO"].ToString();
					CModel tqmts30("TQMTS30");
					CModel hqmts30("HQMTS30");
					tqmts30["MAT_NO"] = tmmsm01["MAT_NO"];
					tqmts30["HEAT_NO"] = " ";
					tqmts30["AREA"] = "北区";
					if (tqmts30.Query("MAT_NO,AREA,HEAT_NO"))
					{
						tqmts30["REC_REVISOR"] = "002104";
						tqmts30["REC_REVISE_TIME"] = s.datetime;
						tqmts30["STATUS_FLAG"] = "4";
						tqmts30.Delete("MAT_NO,AREA,HEAT_NO");
						hqmts30.CopyFrom(tqmts30);
						hqmts30.Insert();
					}
					Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
					tmmsm01.MergeTo(inblock.Tables[0], false);

					tmmsm33dbsx["MAT_NO"] = tmmsm01["MAT_NO"];
					if (tmmsm33dbsx.QueryCount("MAT_NO") > 0)
					{
						tmmsm33dbsx.Delete("MAT_NO");
					}
					tmmsm34["MAT_NO"] = tmmsm01["MAT_NO"];
					tmmsm34["FINISH_FLAG"] = " ";
					if (tmmsm34.QueryCount("MAT_NO,FINISH_FLAG") > 0)
					{
						tmmsm34.Delete("MAT_NO,FINISH_FLAG");
					}
					Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
				}
				else if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3033")//判废取消
				{
					tmmsm96["EVENT_ID"] = "QM06";

					tmmsm96["USAGE_DECISION"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString();
				}
				else if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3006")//改判
				{
					continue;
				}
				else
				{
					sprintf(s.msg, "决策代码无法处理！");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
				tmmsm96["EVENT_LINE_TYPE"] = "00";
				tmmsm96["SYSTEM_ID"] = "QMTS";
				tmmsm96["FUNC_ID"] = s.svc_name;
				tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
				Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
				

				if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3033"
					|| bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3005")
				{
					tmmsm96.MergeTo(tmm39.Tables["TMMSM39"], false);

				}

			}
			else if (hmmsm01.Query("MAT_NO") || hmmsm01.Query("BATCH"))
			{
				if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3001")//综判合格
				{
					hmmsm01["USAGE_DECISION"] = "3001";
					hmmsm01["COMPLEX_DECIDE_CODE"] = "1";
					hmmsm01["COMPLEX_DECIDE_MAKER"] = s.userid;
					hmmsm01["COMPLEX_DECIDE_TIME"] = s.datetime;
					hmmsm01["HOLD_FLAG"] = "0";
					hmmsm01.Update("USAGE_DECISION,COMPLEX_DECIDE_CODE,COMPLEX_DECIDE_MAKER,COMPLEX_DECIDE_TIME,HOLD_FLAG", "MAT_NO");
				}
				else if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3033"&&hmmsm01["C_STATESIGN"].ToString()!="3")//判废取消
				{
					Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
					if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim() == "")
					{
						ud_time = datetime;
					}
					else {
						ud_time = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(0, 4)
							+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(5, 2)
							+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(8, 2)
							+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(11, 2)
							+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(14, 2)
							+ bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["TIMESTAMP"].ToString().Trim().SubstringNE(17, 2);
					}
					tmmsm01["COMPLEX_DECIDE_CODE"] = "9";
					if (!hmmsm01.Query("BATCH,COMPLEX_DECIDE_CODE"))
					{
						sprintf(s.msg, "批次号不存在！");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tmmsm96.CopyFrom(hmmsm01);
					tmmsm96["UD_TIME"] = ud_time;
					tmmsm96["EVENT_ID"] = "QM06";

					tmmsm96["USAGE_DECISION"] = bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString();
					Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);
					tmmsm96["EVENT_LINE_TYPE"] = "00";
					tmmsm96["SYSTEM_ID"] = "QMTS";
					tmmsm96["FUNC_ID"] = s.svc_name;
					tmmsm96.MergeTo(bcls_rec->Tables["MM0099"], false);
					Log::Trace("", __FUNCTION__, "LINKE=[{0}]", __LINE__);


					if (bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3033"
						|| bcls_rec->Tables["ZCHO_QM_UD"].Rows[i]["USAGE_DECISION"].ToString() == "3005")
					{
						tmmsm96.MergeTo(tmm39.Tables["TMMSM39"], false);

					}
				}
			}
			else {
				sprintf(s.msg, "未查到板坯数据！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (miss_E2T8M1.Tables["E2T8M1"].Rows.get_Count() > 0)
		{
			doFlag = f_wmsm_e2t8m1_miss(&miss_E2T8M1, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (inblock.Tables[0].Rows.get_Count() > 0) {
			doFlag = f_wmsm_t8p302_snd(&inblock, bcls_ret, conn);
			if (doFlag < 0) {
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (tmm39.Tables["TMMSM39"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm39_proc(&tmm39, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		if (tmp.Tables[0].Rows.get_Count()>0) {
			doFlag = f_mmsm33dbsx_proc(&tmp, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
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


