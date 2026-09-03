/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-24 19:54:28
Description: 试样信息主表新增 
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

int f_qmts_rep_unsel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//代表成分取消
int f_qmts_21b004_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_t8ed03_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmts_24_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts24("TQMTS24");//试样信息主表
	CModel tqmts24_old("TQMTS24");//试样信息主表
	CModel tqmts23("TQMTS23");
	CModel tqmts24_init("TQMTS24_INIT");
	CDbCommand cmd(conn);
	try
	{
		Log::Trace("", "", "st_sample_no = {0}", bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"].ToString());
		//数据校验
		//tqmts24.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tqmts24["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"];
		tqmts24["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["PLAN_ID"];
		tqmts24["ST_SAMPLE_DIV"] = bcls_rec->Tables[0].Rows[0]["INSPECTION_TYPE"];//T 钢水样 G气体样 I铁样 P铸坯样
		tqmts24["DEV_CODE"] = bcls_rec->Tables[0].Rows[0]["AGGREGATE_NAME"];//工位
		tqmts24["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"];
		tqmts24["SAMPLE_TAKEN_TIME"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME"];
		tqmts24["ST_SAMPLE_SEQ"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_NUMBER"];
		tqmts24["WHOLE_BACKLOG_CODE"] = tqmts24["DEV_CODE"].ToString().Substring(0, 1);;
		tqmts24["SAMPLE_TYPE"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TYPE"];
		tqmts24["ST_NO"] = bcls_rec->Tables[0].Rows[0]["GRADE"];
		tqmts24["SAMPLE_IFGOOD"] = bcls_rec->Tables[0].Rows[0]["BAD_SAMPLE"];
		tqmts24["ANALYSE_TIME"] = bcls_rec->Tables[0].Rows[0]["ANALYSIS_TIME"].ToString();//分析时间
		tqmts24["SAMPLE_TAKEN_TIME"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TAKEN_TIME"].ToString();
		tqmts24["ELEMENTCOUNT"] = bcls_rec->Tables[0].Rows[0]["ELEMENTCOUNT"];//元素个数
		tqmts24["REC_CREATOR"] = s.userid;
		tqmts24["REC_CREATE_TIME"] = datetime;
		tqmts24["ID_ELM"] = bcls_rec->Tables[0].Rows[0]["ID"];
		tqmts24_old.CopyFrom(tqmts24);
		//跟据熔炼号查找制造命令;
		tqmts23["HEAT_NO"] = tqmts24["HEAT_NO"];
		//铁水样查找TPSSM13,TPSSM14表
		if (tqmts24["ST_SAMPLE_DIV"].ToString() == "I")
		{
			sqlstr = " SELECT pono, ST_NO,HEAT_NO  FROM tpssm13 WHERE   heat_no = @heat_no ";
			CDbCommand cmd_tpssm11(conn);
			cmd_tpssm11.SetCommandText(sqlstr);
			cmd_tpssm11.Parameters.Set("heat_no", tqmts23["HEAT_NO"]);
			cmd_tpssm11.ExecuteReader();
			if (cmd_tpssm11.Read())
			{
				cmd_tpssm11.Fetch(tqmts23);
				tqmts24["PONO"] = tqmts23["PONO"];
				tqmts24["ST_NO"] = tqmts23["ST_NO"];
				tqmts24["HEAT_NO"] = tqmts23["HEAT_NO"];
			}
			cmd_tpssm11.Close();
		}
		else if (tqmts24["HEAT_NO"].ToString().Substring(0, 1) == "E")//正常接收电炉数据
		{
			Log::Trace("","","电炉成分");
			//nothing to do
		}
		else
		{
			CModel tpssm12("TPSSM12");
			CModel tpssm42("TPSSM42");
			tpssm12["PROC_NO"] = tqmts23["HEAT_NO"];
			tpssm42["PROC_NO"] = tqmts23["HEAT_NO"];
			tpssm12["PRE_SOLUTION_FLAG"] = "1";
			tpssm42["PRE_SOLUTION_FLAG"] = "1";

			if (!tqmts23.Query("HEAT_NO"))
			{
				Log::Trace("", "", "tpssm11");
				//钢水样查找制造命令和出钢记号
				sqlstr = " SELECT * FROM tpssm11 WHERE heat_no = @heat_no"
					"				UNION"
					"				SELECT * FROM tpssm41 WHERE heat_no = @heat_no ";
				CDbCommand cmd_tpssm11(conn);
				cmd_tpssm11.SetCommandText(sqlstr);
				cmd_tpssm11.Parameters.Set("heat_no", tqmts23["HEAT_NO"]);
				cmd_tpssm11.ExecuteReader();
				if (cmd_tpssm11.Read())
				{
					cmd_tpssm11.Fetch(tqmts23);
				}
				cmd_tpssm11.Close();
			}
			if(tpssm12.QueryCount("PROC_NO,PRE_SOLUTION_FLAG")>0|| tpssm42.QueryCount("PROC_NO,PRE_SOLUTION_FLAG") > 0) {
				Log::Trace("", "", "tpssm12");
				//钢水样查找制造命令和出钢记号
				sqlstr = "SELECT * FROM tpssm11 WHERE heat_no = (select HEAT_NO from vpssm12 where PROC_NO=@heat_no and PRE_SOLUTION_FLAG=1 and ROWNUM=1) "
					" UNION "
					" SELECT * FROM tpssm41 WHERE heat_no = (select HEAT_NO from vpssm12 where PROC_NO =@heat_no and PRE_SOLUTION_FLAG = 1 and ROWNUM = 1) ";
				CDbCommand cmd_tpssm11(conn);
				cmd_tpssm11.SetCommandText(sqlstr);
				cmd_tpssm11.Parameters.Set("heat_no", tqmts23["HEAT_NO"]);
				cmd_tpssm11.ExecuteReader();
				if (cmd_tpssm11.Read())
				{
					cmd_tpssm11.Fetch(tqmts23);
				}
				cmd_tpssm11.Close();
			}
			tqmts24["PONO"] = tqmts23["PONO"];
			tqmts24["ST_NO"] = tqmts23["ST_NO"];
			tqmts24["HEAT_NO"] = tqmts23["HEAT_NO"];
		}
		if (tqmts24["ST_NO"].ToString().Trim() == "" && tqmts24["ST_SAMPLE_DIV"].ToString() != "I"&&!(tqmts24["DEV_CODE"].ToString().SubstringNE(0,1)=="D" && tqmts24["ST_SAMPLE_DIV"].ToString() == "T"))
		{
			sprintf(s.msg, "未查到计划信息");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		tqmts24_old.Query();
		if (tqmts24_old["REP_ELM_SEL_FLAG"].ToString() == "1")
		{
			//当重复接收的时候要先取消代表成分
			EIClass elm_cal;
			elm_cal.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			elm_cal.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
			elm_cal.Tables[0].Rows.Add();
			elm_cal.Tables[0].Rows[0]["HEAT_NO"] = tqmts24_old["HEAT_NO"];
			elm_cal.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts24_old["ST_SAMPLE_NO"];
			doFlag = f_qmts_rep_unsel(&elm_cal, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		map<CString, CString> map_code;
		/*增加TQMTS24横表显示*/
		if (bcls_rec->Tables[0].Rows[0]["INSPECTION_TYPE"].ToString() != "S")//不是渣样
		{
			sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'QMYS2N' ";
		}
		else//渣样
		{
			sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'QMZS2N' ";
		}
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
			if (bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Al")
			{
				tqmts24["ELM_035"] = bcls_rec->Tables[1].Rows[i]["VALUE"];
			}
			tqmts24[elmName] = bcls_rec->Tables[1].Rows[i]["VALUE"];
		}
		/*tqmts24删除新增*/ 
		//C元素无效,或为C样时,不插入24表
		if ((tqmts24["ELM_001"].ToDecimal() < 0&& tqmts24["ST_SAMPLE_DIV"].ToString() != "G") || tqmts24["ST_SAMPLE_DIV"].ToString() == "C" )
		{
			
			Log::Trace("", "", "C元素无效，或为C样时，不插入24表");

		}
		else
		{
			tqmts24.Delete("HEAT_NO,ST_SAMPLE_NO");
			tqmts24.Insert();

			Log::Trace("", "", "C元素不为0时 [{0}]", tqmts24["ELM_001"].ToString());
		}

		Log::Trace("", "", "插入24_init原始表");
		tqmts24_init.CopyFrom(tqmts24);
		tqmts24_init.Delete("HEAT_NO,ST_SAMPLE_NO,ID_ELM");
		tqmts24_init.Insert();

		//发送倒灌站和铁区成分电文
		if (tqmts24["ST_SAMPLE_NO"].ToString().Substring(0, 1) == "H" || tqmts24["ST_SAMPLE_NO"].ToString().Substring(0, 1) == "D")//铁水成分
		{
			EIClass heat_class;
			heat_class.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
			heat_class.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
			heat_class.Tables[0].Rows.Add();
			heat_class.Tables[0].Rows[0]["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
			doFlag = f_qmts_21b004_snd(&heat_class, bcls_rec, conn);
			if (doFlag != 0){
				Log::Trace("", "", "f_qmts_21b004_snd() msg = [{0}]", s.msg);
				s.flag = -1;
				doFlag = -1;
				throw CApplicationException(-1, s.msg, log.Location);
			}
			doFlag = f_t8ed03_snd(&heat_class, bcls_rec, conn);
			if (doFlag != 0){
				Log::Trace("", "", "f_t8ed03_snd() msg = [{0}]", s.msg);
				s.flag = -1;
				doFlag = -1;
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


