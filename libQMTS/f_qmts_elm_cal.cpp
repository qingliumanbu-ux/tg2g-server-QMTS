/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2021-04-01 19:54:28
Description: 复合元素计算，实现TQMTS25表，TQMTQQ1,TQMTQBWQ1表的复合元素计算
调用方式:
bcls_rec->Tables[0].Columns.Add(DT_STRING, "SAMPLE_LOT_NO");  --试批号
bcls_rec->Tables[0].Columns.Add(DT_STRING, "SAMPLE_NO");      --试样号
bcls_rec->Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");	--表名，如果四级计算成品成分 传入TQMTQQ1，三级计算试样成分传入 TQMTS25
bcls_rec->Tables[0].Rows.Add();
bcls_rec->Tables[0].Rows[0]["TABLE_NAME"] = "TQMTQQ1";
bcls_rec->Tables[0].Rows[0]["SAMPLE_LOT_NO"] = "3Q1000501700";
bcls_rec->Tables[0].Rows[0]["SAMPLE_NO"] = "101";
doFlag = f_qmts_elm_cal(bcls_rec, bcls_ret, conn);
if (doFlag != 0)
{
throw CApplicationException(-1, s.msg, log.Location);
}
**************************************************/

#include "stdafx.h"
#include<regex>
BM2_FUNCTION_EXPORT
int f_qmts_elm_cal(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString		table_name = "";	//表名
	CString		heat_no = "";
	CString		st_sample_no = "";
	CString		sample_no = "";
	CString		pono = "";
	CString		st_no = "";
	CString		sample_lot_no = "";
	CDbCommand cmd_upd(conn);
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_dual(conn);
	EIClass elm_act;
	CModel tqmts23("TQMTS23");

	try
	{
		Log::Trace("", "", "11,1 = {0}", bcls_rec->Tables[0].Rows[0]["st_sample_no"].ToString().Substring(10, 1));
		if (bcls_rec->Tables[0].Rows[0]["st_sample_no"].ToString().Substring(11, 1) != "T")//非钢水样
		{
			Log::Trace("", "", "非钢水样");
			return 0;
		}
		table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString().TrimOrBlank().ToUpper();
		if (bcls_rec->Tables[0].Columns.Contains("HEAT_NO"))
		{
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().TrimOrBlank();
			tqmts23["HEAT_NO"] = heat_no;
			if (!tqmts23.Query())
			{
				sqlstr = " SELECT * FROM tpssm11 WHERE heat_no = @heat_no"
					"				UNION"
					"				SELECT * FROM tpssm41 WHERE heat_no = @heat_no ";
				CDbCommand cmd_tpssm11(conn);
				cmd_tpssm11.SetCommandText(sqlstr);
				cmd_tpssm11.Parameters.Set("heat_no", heat_no);
				cmd_tpssm11.ExecuteReader();
				if (cmd_tpssm11.Read())
				{
					cmd_tpssm11.Fetch(tqmts23);
				}
				cmd_tpssm11.Close();
			}
		}
		if (bcls_rec->Tables[0].Columns.Contains("ST_SAMPLE_NO"))
		{
			st_sample_no = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("ST_NO"))
		{
			st_no = bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SAMPLE_LOT_NO"))
		{
			sample_lot_no = bcls_rec->Tables[0].Rows[0]["SAMPLE_LOT_NO"].ToString().TrimOrBlank();
		}
		if (bcls_rec->Tables[0].Columns.Contains("SAMPLE_NO"))
		{
			sample_no = bcls_rec->Tables[0].Rows[0]["SAMPLE_NO"].ToString().TrimOrBlank();
		}

		Log::Trace("", __FUNCTION__, "f_qmts_spe_new IN:---s_table_name		[{0}]", table_name);
		Log::Trace("", __FUNCTION__, "f_qmts_spe_new IN:---s_heat_no			[{0}]", heat_no);
		Log::Trace("", __FUNCTION__, "f_qmts_spe_new IN:---s_st_sample_no		[{0}]", st_sample_no);
		Log::Trace("", __FUNCTION__, "f_qmts_spe_new IN:---tqmts0x.ST_NO		[{0}]", st_no);
		Log::Trace("", __FUNCTION__, "f_qmts_spe_new IN:---s_pono		[{0}]", pono);
		Log::Trace("", __FUNCTION__, "f_qmts_spe_new IN:---sample_lot_no		[{0}]", sample_lot_no);
		Log::Trace("", __FUNCTION__, "f_qmts_spe_new IN:---sample_no		[{0}]", sample_no);


		//校验传入参数
		if (" " == table_name)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数table_name不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (" " == st_sample_no)
		{
			Log::Trace("", __FUNCTION__, "ERROR------[传入参数st_sample_no不允许为空]");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		sqlstr = " DELETE FROM " + table_name +
			"  WHERE ELM_CODE IN (SELECT CODE FROM TEP0002 WHERE CODE_CLASS = 'QMYS2N' AND CODE_DESC_4_CONTENT != ' ') "
			;
		if ("TQMTS25" == table_name)
		{
			sqlstr += " AND ST_SAMPLE_NO = @st_sample_no AND HEAT_NO = @heat_no";
		}
		else if ("TQMTQQ1" == table_name || "TQMTQBWQ1" == table_name)
		{
			sqlstr += " AND SAMPLE_LOT_NO = @sample_lot_no AND SAMPLE_NO =@sample_no";
		}
		cmd_upd.SetCommandText(sqlstr);
		cmd_upd.Parameters.Set("st_sample_no", st_sample_no);
		cmd_upd.Parameters.Set("sample_lot_no", sample_lot_no);
		cmd_upd.Parameters.Set("sample_no", sample_no);
		cmd_upd.Parameters.Set("heat_no", heat_no);
		cmd_upd.ExecuteNonQuery();


		//查找TQMTS25中所有的元素成分
		if ("TQMTS25" == table_name)
		{
			sqlstr = "SELECT ELM_NAME, ELM_ACT,WHOLE_BACKLOG_CODE FROM TQMTS25 WHERE ST_SAMPLE_NO = @st_sample_no AND HEAT_NO =@heat_no ORDER BY LENGTH(ELM_NAME) DESC ";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_sample_no", st_sample_no);
			cmd_inq.Parameters.Set("heat_no", heat_no);
			cmd_inq.ExecuteQuery(elm_act.Tables[0]);
		}
		//查找组合元素公式
		sqlstr = " SELECT"
			"			CODE AS ELM_CODE,"
			"			CODE_DESC_1_CONTENT AS ELM_NAME,"
			"			CODE_DESC_4_CONTENT"
			"			FROM"
			"			TEP0002"
			"			WHERE"
			"			CODE_CLASS = 'QMYS2N'"
			"			AND CODE_DESC_4_CONTENT <> ' '";
		/*"			AND CODE IN("
		"			SELECT"
		"			ELM_CODE"
		"			FROM"
		"			TQMTS02"
		"			WHERE"
		"			IDX_NO IN("
		"			SELECT"
		"			ELM_STD_IDX_A"
		"			FROM"
		"			TQMTS0X"
		"			WHERE"
		"			ST_NO = @st_no)) "*/
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Set("st_no", tqmts23["ST_NO"]);
		cmd_inq.ExecuteReader();
		while (cmd_inq.Read())
		{
			cmd_dual.Close();
			CString elm_code = cmd_inq.GetString(1);
			CString elm_name = cmd_inq.GetString(2);
			CString elm_complex = cmd_inq.GetString(3);
			elm_complex = elm_complex.Replace(" ", "");//空格全部替换掉
			for (size_t i = 0; i < elm_act.Tables[0].Rows.get_Count(); i++)
			{
				//Log::Trace("", "", "转换前：elm_complex = {0}", elm_complex);
				elm_complex = elm_complex.Replace(elm_act.Tables[0].Rows[i]["ELM_NAME"].ToString(), elm_act.Tables[0].Rows[i]["ELM_ACT"].ToString());
				//Log::Trace("", "", "转换后：elm_complex = {0}", elm_complex);
			}
			string dest = string((const char*)elm_complex);
			regex pattern("[a-zA-z]");
			bool is_match = regex_search(dest, pattern);
			if (!is_match)//没找到
			{
				regex pattern0("[\\/][0][^\\.]");
				bool is_match0 = regex_search(dest, pattern0);
				if (!is_match0)//未找到，说明不存在/0的格式
				{
					regex pattern0_end("[\\/][0]$");
					bool is_match0_end = regex_search(dest, pattern0_end);
					if (!is_match0_end)//末尾不为/0
					{
						sqlstr = "SELECT ROUND(" + dest + ",5) FROM DUAL";
						cmd_dual.SetCommandText(sqlstr);
						try
						{
							cmd_dual.ExecuteReader();
							if (cmd_dual.Read())
							{
								if ("TQMTS25" == table_name)
								{
									sqlstr = "INSERT INTO TQMTS25 (PONO,HEAT_NO,ST_SAMPLE_NO,ST_NO,ELM_CODE,ELM_NAME,ELM_ACT,ELM_ACT_OLD,ELM_OK,REC_CREATOR,REC_CREATE_TIME,WHOLE_BACKLOG_CODE) "
										" VALUES(@pono,@heat_no,@st_sample_no,@st_no,@tqmts02.ELM_CODE,@tqmts02.ELM_NAME,@elm_value,@elm_value,'0',@rec_creator,@rec_create_time,@whole_backlog_code) ";
									cmd_upd.SetCommandText(sqlstr);
									cmd_upd.Parameters.Set("pono", tqmts23["PONO"]);
									cmd_upd.Parameters.Set("heat_no", heat_no);
									cmd_upd.Parameters.Set("st_sample_no", st_sample_no);
									cmd_upd.Parameters.Set("st_no", tqmts23["ST_NO"]);
									cmd_upd.Parameters.Set("tqmts02.ELM_CODE", elm_code);
									cmd_upd.Parameters.Set("tqmts02.ELM_NAME", elm_name);
									CDecimal elm_value = cmd_dual.GetDecimal(1);
									cmd_upd.Parameters.Set("elm_value", elm_value);
									cmd_upd.Parameters.Set("rec_creator", "DBCOUNT");
									cmd_upd.Parameters.Set("rec_create_time", datetime);
									cmd_upd.Parameters.Set("whole_backlog_code", elm_act.Tables[0].Rows[0]["WHOLE_BACKLOG_CODE"].ToString());
									Log::Trace("", "", "elm_name = {0}", elm_name);
									try
									{
										cmd_upd.ExecuteNonQuery();
									}
									catch (CDbException &ex)
									{
										// 超长数据、除以0的数据就不存入了
										if (ex.GetCode() != 1438 && ex.GetCode() != 1476)
										{
											CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
											CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
											CString str = sqlstr + "\r\n" + ex.GetMsg();
											strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
											s.flag = -1;
											doFlag = -1;
										}
									}
								}
							}
						}
						catch (CDbException &ex)
						{
							// 超长数据、除以0的数据就不存入了
							if (ex.GetCode() != 1438 && ex.GetCode() != 1476)
							{
								CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
								CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
								CString str = sqlstr + "\r\n" + ex.GetMsg();
								strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
								s.flag = -1;
								doFlag = -1;
							}
						}
					}
				}
				else
				{
					Log::Trace("", "", "line = {0},有错误算法不计算", __LINE__);
					Log::Trace("", "", "elm_complex = {0}", elm_complex);
					continue;
				}
			}
			else
			{
				Log::Trace("", "", "line = {0},缺少元素不计算", __LINE__);
				Log::Trace("", "", "elm_complex = {0}", elm_complex);
				continue;
			}
		}
		cmd_inq.Close();
		cmd_upd.Close();
		//正则匹配是否有A-Z
		/*string dest = "(1+1+1)-1*55/3";
		regex pattern("[a-zA-z]");
		bool is_match = regex_search(dest, pattern);*/

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


