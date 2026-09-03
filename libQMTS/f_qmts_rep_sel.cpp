/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-28 19:54:28
Description: 代表成分选定
1.新增TQMTS29表，TQMTSB0表
2.修改TQMTS24代表成分标记
3.修改炉次质量信息表TQMTS23代表成分标记
4.修改TPSSM13，TPSSM11表的代表成分标记
5.检查元素是否齐全
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

int f_t82305_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给智慧质量发电文
int f_210010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//给L4发送电文
int f_push_baowu_chat(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//发送宝武聊天
int f_qmts_30_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmts_rep_sel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString str = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts29("TQMTS29");
	CModel tqmtsb0("TQMTSB0");
	CModel tqmts24("TQMTS24");
	CModel tqmts23("TQMTS23");
	CModel tpssm13("TPSSM13");
	CModel tpssm11("TPSSM11");
	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inqph(conn);

	try
	{
		Log::Trace("", "", "st_sample_no = {0}", bcls_rec->Tables[0].Rows[0]["st_sample_no"].ToString());
		CString table_name = " ";
		if (bcls_rec->Tables[0].Columns.Contains("TABLE_NAME"))
			table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString();
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		CString st_sample_no = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString();
		Log::Trace("", "", "st_sample_no ={0}", st_sample_no);
		tqmts24["HEAT_NO"] = heat_no;
		tqmts24["ST_SAMPLE_NO"] = st_sample_no;
		tqmts24.Query();
		Log::Trace("", "", "成分判定结果,{0}", tqmts24["JUDGE_CODE"].ToString());
		tqmts23["HEAT_NO"] = tqmts24["HEAT_NO"];
		tqmts23.Query("HEAT_NO");
		if (tqmts24["WHOLE_BACKLOG_CODE"].ToString() != "C")//非钢水样
		{
			Log::Trace("", "", "不是连铸样");
			sprintf(s.msg, "只有连铸样可以作为代表样");
			return 0;
		}
		if (tqmts24["ST_SAMPLE_DIV"].ToString() != "T"&& tqmts24["ST_SAMPLE_DIV"].ToString() != "P" && tqmts24["ST_SAMPLE_DIV"].ToString() != "1")//非钢样,非坯样
		{
			Log::Trace("", "", "不是钢样，不是坯样");
			sprintf(s.msg, "只有钢样和坯样可以作为代表样");
			return 0;
		}
		Log::Trace("", "", "REP_ELM_SEL_FLAG,[{0}]", tqmts23["REP_ELM_SEL_FLAG"].ToString());

		Log::Trace("", "", "开始选择代表成分");
		/*清空TQMTS24表代表成分标记*/
		tqmts24["REP_ELM_SEL_FLAG"] = " ";
		tqmts24.Update("REP_ELM_SEL_FLAG", "HEAT_NO");
		/*调整标记*/
		tqmts24["REP_ELM_SEL_FLAG"] = "1";
		tqmts24.Update("REP_ELM_SEL_FLAG", "HEAT_NO,ST_SAMPLE_NO");

		tqmts23["REP_ELM_SEL_FLAG"] = "1";
		tqmts23.Update("REP_ELM_SEL_FLAG", "HEAT_NO");
		tpssm13["HEAT_NO"] = tqmts24["HEAT_NO"];
		tpssm13["REP_ELM_SEL_FLAG"] = "1";
		tpssm13.Update("REP_ELM_SEL_FLAG", "HEAT_NO");
		tpssm11["HEAT_NO"] = tqmts24["HEAT_NO"];
		tpssm11["REP_ELM_SEL_FLAG"] = "1";
		tpssm11.Update("REP_ELM_SEL_FLAG", "HEAT_NO");
		tqmtsb0.CopyFrom(tqmts24);
		tqmtsb0.Delete("HEAT_NO");
		tqmtsb0.Insert();
		tqmts29["HEAT_NO"] = tqmts24["HEAT_NO"];
		tqmts29.Delete("HEAT_NO");

		//新增TQMTS29表
		sqlstr = " SELECT * FROM tqmts25 WHERE  HEAT_NO = @heat_no"
			"				AND ST_SAMPLE_NO = @st_sample_no ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.Parameters.Set("st_sample_no", st_sample_no);
		cmd.ExecuteReader();
		while (cmd.Read())
		{
			cmd.Fetch(tqmts29);
			tqmts29["REC_CREATOR"] = s.userid;
			tqmts29["REC_CREATE_TIME"] = datetime;
			tqmts29.Insert();
		}

		//新增PM_SAMPLE表
		sqlstr = "DELETE FROM PM_SAMPLE  WHERE HEATNUMBER = @heat_no";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteNonQuery();
		sqlstr = " INSERT"
			"				INTO"
			"				PM_SAMPLE("
			"				ID,"
			"				PLANID,"
			"				SAMPLETIME,"
			"				ANALYSISTIME,"
			"				SAMPLEID,"
			"				HEATNUMBER,"
			"				AGGREGATECODE,"
			"				GRADEID,"
			"				ELEMENTCOUNT,"
			"				SAMPLENUMBER,"
			"				C,"
			"				SI,"
			"				MN,"
			"				P,"
			"				S,"
			"				AL,"
			"				CU,"
			"				NI,"
			"				CR,"
			"				ARSENIC,"
			"				SN,"
			"				NB,"
			"				V,"
			"				TI,"
			"				MO,"
			"				B,"
			"				W,"
			"				CA,"
			"				H,"
			"				O,"
			"				N,"
			"				CO,"
			"				ZR,"
			"				SB,"
			"				MG,"
			"				FE,"
			"				ZN,"
			"				SE,"
			"				TE,"
			"				PB,"
			"				BI,"
			"				CRNIEQ,"
			"				ALS,"
			"				TA"
			"				)"
			""
			"				SELECT"
			"				ID_ELM AS ID,"
			"				HEAT_NO AS PLANID,"
			"				SYSDATE AS SAMPLETIME,"
			"				SYSDATE AS ANALYSISTIME,"
			"				ST_SAMPLE_NO AS SAMPLEID,"
			"				HEAT_NO AS HEATNUMBER,"
			"				DEV_CODE AS AGGREGATECODE,"
			"				ST_NO AS GRADEID,"
			"				ELEMENTCOUNT AS ELEMENTCOUNT,"
			"				ST_SAMPLE_SEQ AS SAMPLENUMBER,"
			"				ELM_001 AS C,"
			"				ELM_002 AS SI,"
			"				ELM_003 AS MN,"
			"				ELM_004 AS P,"
			"				ELM_005 AS S,"
			"				ELM_010 AS AL,"
			"				ELM_009 AS CU,"
			"				ELM_007 AS NI,"
			"				ELM_006 AS CR,"
			"				ELM_021 AS ARSENIC,"
			"				ELM_023 AS SN,"
			"				ELM_011 AS NB,"
			"				ELM_012 AS V,"
			"				ELM_013 AS TI,"
			"				ELM_008 AS MO,"
			"				ELM_018 AS B,"
			"				ELM_019 AS W,"
			"				ELM_020 AS CA,"
			"				ELM_014 AS H,"
			"				ELM_015 AS O,"
			"				ELM_016 AS N,"
			"				ELM_017 AS CO,"
			"				ELM_026 AS ZR,"
			"				ELM_024 AS SB,"
			"				ELM_029 AS MG,"
			"				ELM_030 AS FE,"
			"				ELM_114 AS ZN,"
			"				ELM_113 AS SE,"
			"				ELM_031 AS TE,"
			"				ELM_022 AS PB,"
			"				ELM_025 AS BI,"
			"				ELM_041 AS CRNIEQ,"
			"				ELM_036 AS ALS,"
			"				ELM_033 AS TA"
			"				FROM"
			"				TQMTSB0"
			"				WHERE HEAT_NO = @heat_no ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteNonQuery();

		cmd.Close();
		//发送智慧质量电文
		doFlag = f_t82305_snd(bcls_rec, bcls_ret, conn);
		if (doFlag != 0){
			Log::Trace("", "", "f_t82302_snd() msg = [{0}]", s.msg);
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//发送L4电文
		doFlag = f_210010_snd(bcls_rec, bcls_ret, conn);
		if (doFlag != 0){
			Log::Trace("", "", "f_210010_snd() msg = [{0}]", s.msg);
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//调用处置表信息
		doFlag = f_qmts_30_ins(bcls_rec, bcls_ret, conn);
		if (doFlag != 0){
			Log::Trace("", "", "f_qmts_30_ins() msg = [{0}]", s.msg);
			s.flag = -1;
			doFlag = -1;
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//不合格代表样发送宝武聊天
		if (tqmts24["JUDGE_CODE"].ToString() == "2" && tqmts24["REP_ELM_SEL_FLAG"].ToString() == "1")//判定不合格与为代表成分的则发宝武聊天推送
		{
			//查询牌号
			Log::Trace("", "", "ST_NO = {0}", tqmts24["ST_NO"].ToString());
			CString sqlstrph = "SELECT SG_GRADE_1 FROM TQMTS0X  WHERE ST_NO = @st_no";
			cmd_inqph.SetCommandText(sqlstrph);
			cmd_inqph.Parameters.Set("st_no", tqmts24["ST_NO"].ToString());
			cmd_inqph.ExecuteReader();
			CString sg_grade = "";
			if (cmd_inqph.Read())
			{
				sg_grade = cmd_inqph.GetString(1);
			}
			cmd_inqph.Close();
			sqlstr = "SELECT CODE_DESC_1_CONTENT AS  ELM_ENAME, "
				"MAIN_MIN, MAIN_MAX, T2.ELM_ACT_OLD  FROM TQMTS0X LEFT JOIN TQMTS02 "
				"ON TQMTS0X.ELM_STD_IDX_A = TQMTS02.IDX_NO "
				"LEFT JOIN(SELECT * FROM TEP0002 WHERE CODE_CLASS = 'QMYS2N') T1 ON TQMTS02.ELM_CODE = T1.CODE "
				"LEFT JOIN(SELECT * FROM TQMTS25 WHERE ST_SAMPLE_NO = '" + st_sample_no + "') T2 ON TQMTS02.ELM_CODE = T2.ELM_CODE "
				" WHERE TQMTS0X.ST_NO = '" + tqmts24["ST_NO"].ToString() + "'  AND T2.ELM_OK = '1' ";

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();
			str += "<br>熔炼号[" + heat_no + "]，";
			str += "钢种[" + tqmts24["ST_NO"].ToString() + "]，";
			str += "试样号[" + st_sample_no + "]，";
			str += "牌号[" + sg_grade + "]，";
			while (cmd_inq.Read())
			{
				str += cmd_inq.GetString(1) + "不合格，";
				str += "元素范围[" + cmd_inq.GetDecimal(2).ToString();
				str += "-" + cmd_inq.GetDecimal(3).ToString() + "]，";
				str += "实际[" + cmd_inq.GetDecimal(4).ToString() + "]";
			}
			cmd_inq.Close();
			EIClass bcls_rec_s;
			bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "CODE");
			bcls_rec_s.Tables.Add();
			bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK");
			bcls_rec_s.Tables.Add();
			bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "REMARK_CF");

			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["CODE"] = "7";
			bcls_rec_s.Tables[0].Rows[0]["REMARK"] = st_sample_no;
			bcls_rec_s.Tables[0].Rows[0]["REMARK_CF"] = str;
			doFlag = f_push_baowu_chat(&bcls_rec_s, bcls_ret, conn);
			if (doFlag < 0)
			{
				doFlag = 0;
				s.flag = 0;
				sprintf(s.msg, "");
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


