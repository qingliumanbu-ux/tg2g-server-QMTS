/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-24 19:54:28
Description: 成分接收函数，后备新增画面调用函数
1.为提高可读性，将判断放到各个函数中
2.一个函数尽量只做一件事情
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

int f_qmts_24_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//试样主表TQMTS24表新增
int f_qmts_25_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//试样子表TQMTS25表新增
int f_qmts_26_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//试样子表，渣样TQMTS26表新增
int f_qmts_elm_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//成分判定
int f_qmts_call_judge(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);//消息规则引擎
int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

int f_qmts_elm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	CModel tqmts24("TQMTS24");
	CModel tqmts25("TQMTS25");
	CDbCommand cmd(conn);
	EIClass in_23m;
	in_23m.Tables[0].Columns.Add(DT_STRING, "TC_NO");
	in_23m.Tables[0].Rows.Add();
	in_23m.Tables[0].Rows[0]["TC_NO"] = "T82312";
	in_23m.Tables.Add();
	in_23m.Tables[1].Columns.Add(tqmts24);
	in_23m.Tables.Add();
	in_23m.Tables[2].Columns.Add(tqmts25);

	try
	{
		if (bcls_rec->Tables[0].Rows[0]["BAD_SAMPLE"].ToString() == "1")
		{
			Log::Trace("","","坏样不接收");
			CModel tqmts24_init("TQMTS24_INIT");
			tqmts24_init["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["PLAN_ID"];
			tqmts24_init["ST_SAMPLE_DIV"] = bcls_rec->Tables[0].Rows[0]["INSPECTION_TYPE"];//T 钢水样 G气体样 I铁样 P铸坯样
			tqmts24_init["DEV_CODE"] = bcls_rec->Tables[0].Rows[0]["AGGREGATE_NAME"];//工位
			tqmts24_init["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"];
			tqmts24_init["SAMPLE_TAKEN_TIME"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME"];
			tqmts24_init["ST_SAMPLE_SEQ"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_NUMBER"];
			tqmts24_init["WHOLE_BACKLOG_CODE"] = bcls_rec->Tables[0].Rows[0]["AGGREGATE_NAME"];
			tqmts24_init["SAMPLE_TYPE"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TYPE"];
			tqmts24_init["ST_NO"] = bcls_rec->Tables[0].Rows[0]["GRADE"];
			tqmts24_init["SAMPLE_IFGOOD"] = bcls_rec->Tables[0].Rows[0]["BAD_SAMPLE"];
			tqmts24_init["ANALYSE_TIME"] = bcls_rec->Tables[0].Rows[0]["ANALYSIS_TIME"].ToString();//分析时间
			tqmts24_init["SAMPLE_TAKEN_TIME"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME"].ToString();
			tqmts24_init["ELEMENTCOUNT"] = bcls_rec->Tables[0].Rows[0]["ELEMENTCOUNT"];//元素个数
			tqmts24_init["REC_CREATOR"] = s.userid;
			tqmts24_init["REC_CREATE_TIME"] = datetime;
			tqmts24_init["ID_ELM"] = bcls_rec->Tables[0].Rows[0]["ID"];
			tqmts24_init["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"];
			tqmts24_init.Insert();
			return 0;
		}
		//合格成分，同一个试样号，不接收
		CModel tqmts24_old("TQMTS24");//试样信息主表
		tqmts24_old["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"];
		tqmts24_old.Query("ST_SAMPLE_NO");
		if (tqmts24_old["REP_ELM_SEL_FLAG"].ToString() == "1" && tqmts24_old["JUDGE_CODE"].ToString() == "1")//代表成分且合格
		{
			Log::Trace("", "", "代表成分且合格");
			CModel tqmts24_init("TQMTS24_INIT");
			tqmts24_old["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["PLAN_ID"];
			tqmts24_old["ST_SAMPLE_DIV"] = bcls_rec->Tables[0].Rows[0]["INSPECTION_TYPE"];//T 钢水样 G气体样 I铁样 P铸坯样
			tqmts24_old["DEV_CODE"] = bcls_rec->Tables[0].Rows[0]["AGGREGATE_NAME"];//工位
			tqmts24_old["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"];
			tqmts24_old["SAMPLE_TAKEN_TIME"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME"];
			tqmts24_old["ST_SAMPLE_SEQ"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_NUMBER"];
			tqmts24_old["WHOLE_BACKLOG_CODE"] = tqmts24_old["DEV_CODE"].ToString().Substring(0, 1);;
			tqmts24_old["SAMPLE_TYPE"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TYPE"];
			tqmts24_old["ST_NO"] = bcls_rec->Tables[0].Rows[0]["GRADE"];
			tqmts24_old["SAMPLE_IFGOOD"] = bcls_rec->Tables[0].Rows[0]["BAD_SAMPLE"];
			tqmts24_old["ANALYSE_TIME"] = bcls_rec->Tables[0].Rows[0]["ANALYSIS_TIME"].ToString();//分析时间
			tqmts24_old["SAMPLE_TAKEN_TIME"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME"].ToString();
			tqmts24_old["ELEMENTCOUNT"] = bcls_rec->Tables[0].Rows[0]["ELEMENTCOUNT"];//元素个数
			tqmts24_old["REC_CREATOR"] = s.userid;
			tqmts24_old["REC_CREATE_TIME"] = datetime;
			tqmts24_old["ID_ELM"] = bcls_rec->Tables[0].Rows[0]["ID"];
			map<CString, CString> map_code;
			CDbCommand cmd(conn);
			sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'QMYS2N' ";
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteReader();
			while (cmd.Read())
			{
				map_code.insert(pair<CString, CString>(cmd.GetString(2), cmd.GetString(1)));
			}
			cmd.Close();
			for (size_t i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
			{
				CString elmName = "ELM_" + map_code[bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString()];
				if (bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Fe%" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Alsol" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Aloxy" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "AlsolE" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "AloxyE" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "F" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "E" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Alinsol")
				{
					Log::Trace("", "", "[{0}]元素跳过", bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString());
					continue;
				}
				if (elmName == "ELM_")
				{
					sprintf(s.msg, "[" + bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() + "]元素未在QMYS2N小代码中找到对应关系");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tqmts24_old[elmName] = bcls_rec->Tables[1].Rows[i]["VALUE"];
			}
			tqmts24_init.CopyFrom(tqmts24_old);
			tqmts24_init.Delete("HEAT_NO,ST_SAMPLE_NO,ID_ELM");
			tqmts24_init.Insert();
			//清空太钢特有处置表
			CModel tqmts30("TQMTS30");
			tqmts30["HEAT_NO"] = tqmts24_old["HEAT_NO"];
			tqmts30["ST_SAMPLE_NO"] = tqmts24_old["ST_SAMPLE_NO"];
			tqmts30["AREA"] = "北区";
			tqmts30["MAT_NO"] = " ";
			if (tqmts30.Query())
			{
				if (tqmts30["DECIDER"].ToString() != "系统")//手动新增的TQMTS30数据后，就不让连铸结果覆盖了
				{
					return 0;
				}
			}
			tqmts30.Delete("HEAT_NO,AREA,MAT_NO");
			return 0;
		}

		//调用试样成分新增函数，新增TQMTS24表
		doFlag = f_qmts_24_ins(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//调用试样成分新增函数，新增TQMTS25表
		doFlag = f_qmts_25_ins(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//调用渣样成分新增函数，新增TQMTS26表
		doFlag = f_qmts_26_ins(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//成分判定
		EIClass iblk_lh;
		iblk_lh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		iblk_lh.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
		iblk_lh.Tables[0].Rows.Add();
		iblk_lh.Tables[0].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"].ToString();
		iblk_lh.Tables[0].Rows[0]["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"].ToString();
		doFlag = f_qmts_elm_jud(&iblk_lh, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//规则引擎
		EIClass iblk_yq;
		if (iblk_yq.Tables.Contains("RULE_CONFIG") == false)
		{
			iblk_yq.Tables[0].set_TableName("RULE_CONFIG");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "PROJECT_ENAME");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "ENV_TYPE");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "VERSION");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "CUSTOM_CONFIG");
			iblk_yq.Tables["RULE_CONFIG"].Columns.Add(DT_STRING, "CODE_CLASS");
			iblk_yq.Tables["RULE_CONFIG"].Rows.Add();
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["PROJECT_ENAME"] = "TASK";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["ENV_TYPE"] = "0";//测试--0，正式--1
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["VERSION"] = "20241201";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["CUSTOM_CONFIG"] = "T";//固定值，不变
			iblk_yq.Tables["RULE_CONFIG"].Rows[0]["CODE_CLASS"] = "EPIJG0";//固定值，不变
		}
		if (iblk_yq.Tables.Contains("PROJECT_CONFIG") == false)
		{
			iblk_yq.Tables.Add();
			iblk_yq.Tables[1].set_TableName("PROJECT_CONFIG");
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "MESSAGE_CLASS");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "UNIT_CODE");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "MAT_NO");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "ST_NO");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "HEAT_NO");//三列必须有
			iblk_yq.Tables["PROJECT_CONFIG"].Columns.Add(DT_STRING, "DEV_CODE");//三列必须有
		}
		if (iblk_yq.Tables.Contains("DATA_CUSTOM") == false)
		{
			Log::Trace("", "", "inBlock 初始化表3为 DATA_CUSTOM ");

			iblk_yq.Tables.Add("DATA_CUSTOM");
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "UNIT_CODE");//该列必须有
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "HEAT_NO");//参与计算的列
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "ST_SAMPLE_NO");//参与计算的列
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "ST_NO");//参与计算的列
			iblk_yq.Tables["DATA_CUSTOM"].Columns.Add(DT_STRING, "DEV_CODE");//参与计算的列
		}

		iblk_yq.Tables["PROJECT_CONFIG"].Rows.Add();
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["MESSAGE_CLASS"] = "QMTS_ELM";//任务池准入条件的比对值
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["UNIT_CODE"] = "H000";//不定机组
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["ST_NO"] = bcls_rec->Tables[0].Rows[0]["GRADE"].ToString();
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"].ToString();
		iblk_yq.Tables["PROJECT_CONFIG"].Rows[0]["DEV_CODE"] = bcls_rec->Tables[0].Rows[0]["AGGREGATE_NAME"].ToString();


		iblk_yq.Tables["DATA_CUSTOM"].Rows.Add();
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["UNIT_CODE"] = "H000";//不定机组
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"].ToString();
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"].ToString();
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["ST_NO"] = bcls_rec->Tables[0].Rows[0]["GRADE"].ToString();
		Log::Trace("", "", "st_no={0}", bcls_rec->Tables[0].Rows[0]["GRADE"].ToString());
		iblk_yq.Tables["DATA_CUSTOM"].Rows[0]["DEV_CODE"] = bcls_rec->Tables[0].Rows[0]["AGGREGATE_NAME"].ToString();
		Log::Trace("", "", "dev_code={0}", bcls_rec->Tables[0].Rows[0]["AGGREGATE_NAME"].ToString());
		doFlag = f_qmts_call_judge(&iblk_yq, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "引擎失败 ");
			doFlag = 0;
			s.flag = 0;
		}
		Log::Trace("", "", "引擎成功 ");

		tqmts24["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"].ToString();
		tqmts25["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"].ToString();
		tqmts24["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"].ToString();
		tqmts25["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"].ToString();
		tqmts24.Query("HEAT_NO,ST_SAMPLE_NO");
		tqmts24.MergeTo(in_23m.Tables[1]);
		if (bcls_rec->Tables[0].Rows[0]["INSPECTION_TYPE"].ToString() == "S")
		{
			sqlstr = " select * from tqmts26 where HEAT_NO='" + tqmts25["HEAT_NO"].ToString() + "' and ST_SAMPLE_NO='" + tqmts25["ST_SAMPLE_NO"].ToString() + "' ";
		}
		else
		{
			sqlstr = " select * from tqmts25 where HEAT_NO='" + tqmts25["HEAT_NO"].ToString() + "' and ST_SAMPLE_NO='" + tqmts25["ST_SAMPLE_NO"].ToString() + "' ";
		}
		
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteReader();
		while (cmd.Read())
		{
			cmd.Fetch(tqmts25);
			tqmts25.MergeTo(in_23m.Tables[2]);
		}
		cmd.Close();

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


