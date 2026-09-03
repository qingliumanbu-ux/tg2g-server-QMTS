/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      panchen
Version:     1.0
Date:        2024-04-18 19:54:28
Description: 板坯待判处置新增
**************************************************/

#include "stdafx.h" 

BM2_FUNCTION_EXPORT


int f_qmts_30_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	CModel tqmts30("TQMTS30");
	CModel hqmts30("HQMTS30");
	CModel tqmts24("TQMTS24");
	CModel tqmts0x("TQMTS0X");
	try
	{
		//清空太钢特有处置表
		tqmts30["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];
		tqmts30["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"];
		tqmts30["AREA"] = "北区";
		tqmts30["MAT_NO"] = " ";
		if (tqmts30.Query())
		{
			if (tqmts30["DECIDER"].ToString() != "系统")//手动新增的TQMTS30数据后，就不让连铸结果覆盖了
			{
				Log::Trace("", "", "手动新增数据，返回");
				return 0;
			}
			if (tqmts30["STATUS_FLAG"].ToString() != "0")//审核或者其他操作以后就返回
			{
				Log::Trace("", "", "已审核数据，返回");
				return 0;
			}
			if ((CString)s.svc_name == "qmbs25s_upd")//周天成要求，当手动修改成分表时，不清除TQMTS30表
			{
				Log::Trace("", "", "手动修改，返回");
				return 0;
			}
		}
		sqlstr = "SELECT * FROM TQMTS24 WHERE   HEAT_NO = @heat_no  AND REP_ELM_SEL_FLAG = '1'";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", tqmts30["HEAT_NO"]);
		cmd.ExecuteReader();
		if (cmd.Read());
		{
			cmd.Fetch(tqmts24);
		}
		tqmts30.Delete("HEAT_NO,AREA,MAT_NO");
		if (tqmts24["JUDGE_CODE"].ToString() == "1")
		{
			Log::Trace("", "", "成分合格，返回");
			return 0;
		}

		tqmts30["FULL_FURNACE"] = "是";
		tqmts30["REC_CREATOR"] = s.userid;
		tqmts30["REC_CREATE_TIME"] = datetime;
		tqmts30["AREA"] = "北区";
		tqmts30["STATUS_FLAG"] = "0";
		tqmts30["REMARK_3"] = "0";
		tqmts30["REPORT_TIME"] = datetime;
		tqmts30["DECIDER"] = "系统";
		tqmts30["REASON_1"] = tqmts24["REMARK"];
		sqlstr = "SELECT sum(SLAB_WT),count(*),max(SLAB_THICK),max(SLAB_WIDTH) FROM tmmsm33 WHERE HEAT_NO = @heat_no";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", tqmts30["HEAT_NO"].ToString());
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			tqmts30["MAT_THEORY_WT"] = cmd.GetDecimal(1);
			tqmts30["MAT_NUM"] = cmd.GetDecimal(2);
			tqmts30["MAT_ACT_THICK"] = cmd.GetDecimal(3);
			tqmts30["MAT_ACT_WIDTH"] = cmd.GetDecimal(4);
		}

		//2025.09.29 在线表没有数据
		sqlstr = "SELECT C_DIV FROM VMMSM01 WHERE HEAT_NO = @heat_no ORDER BY REC_CREATE_TIME DESC";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", tqmts30["HEAT_NO"].ToString());
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			tqmts30["C_DIV"] = cmd.GetString(1);
		}


		//出钢记号
		tqmts24["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];
		tqmts24["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"];
		tqmts24.Query();
		tqmts0x["ST_NO"] = tqmts24["ST_NO"];
		tqmts0x.Query("ST_NO");
		tqmts30["SG_GRADE_1"] = tqmts0x["SG_GRADE_1"];
		tqmts30["ST_NO"] = tqmts0x["ST_NO"];
		tqmts30["PROD_DATE"] = tqmts24["SAMPLE_TAKEN_TIME"].ToString().Substring(0,8);
		//不合格元素
		CString reason = "";
		sqlstr = " SELECT T2.ELM_NAME, DECODE((decode(SUBSTR(ELM_ACT,1,1),'.','0'||ELM_ACT,ELM_ACT)), NULL,'无检验或缺失', (decode(SUBSTR(ELM_ACT,1,1),'.','0'||ELM_ACT,ELM_ACT))), SPE_MIN, SPE_MAX, T1.ELM_OK FROM(SELECT * FROM tqmts25 WHERE ST_SAMPLE_NO = @st_sample_no) T1"
			"			RIGHT  JOIN(SELECT * FROM TQMTS02 WHERE IDX_NO IN(SELECT ELM_STD_IDX_A FROM TQMTS0X WHERE ST_NO = @st_no)) T2 ON   T1.ELM_CODE = T2.ELM_CODE"
			"			WHERE (ELM_OK = '1' OR ELM_OK IS NULL) ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("st_sample_no", tqmts30["ST_SAMPLE_NO"].ToString());
		cmd.Parameters.Set("st_no", tqmts24["ST_NO"].ToString());
		cmd.ExecuteReader();
		while (cmd.Read())
		{
			reason += "【";
			reason += cmd.GetString(1);
			reason += "】";
			reason += cmd.GetString(2);
			reason += ",元素范围【" + cmd.GetDecimal(3).ToString();
			reason += "-" + cmd.GetDecimal(4).ToString() + "】";
		}
		tqmts30["REASON_1"] = reason;
		if (tqmts30["REASON_1"].ToString().GetLength() > 499)
		{
			tqmts30["REASON_1"] = tqmts30["REASON_1"].ToString().Substring(0, 500);
		}
		tqmts30.TrimOrBlank();
		tqmts30.Insert();
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


