/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-01-25 9:13:28
Description: 成分发送L4
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_210010_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	EIClass tqmts29;
	CModel tqmts0x("TQMTS0X");
	CModel tqmtsfgb("TQMTSFGB");
	try
	{
		CString epex_number = "210010";
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("tqmtqb0", "heat_no", 0, heat_no) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		CDbCommand cmd(conn);
		sqlstr = " SELECT TQMTS29.ST_NO, TQMTS29.PONO, TQMTS29.ELM_CODE, TQMTS29.ELM_NAME, TQMTS29.ELM_ACT FROM tep0002"
			"			LEFT JOIN TQMTS29"
			"			ON"
			"			tep0002.CODE = TQMTS29.ELM_CODE"
			"			WHERE tep0002.CODE_CLASS = 'QMYS2N'"
			"			AND tep0002.CODE IN('001',"
			"			'002',"
			"			'003',"
			"			'004',"
			"			'005',"
			"			'006',"
			"			'007',"
			"			'008',"
			"			'009',"
			"			'010',"
			"			'011',"
			"			'012',"
			"			'013',"
			"			'014',"
			"			'015',"
			"			'016',"
			"			'017',"
			"			'018',"
			"			'019',"
			"			'020',"
			"			'021',"
			"			'022',"
			"			'023',"
			"			'024',"
			"			'025',"
			"			'026',"
			"			'029',"
			"			'030',"
			"			'031',"
			//2025.06.06新增032元素Ce
			"			'032',"	
			"			'033',"
			"			'035',"
			"			'036')"
			"			AND TQMTS29.HEAT_NO = @heat_no ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteQuery(tqmts29.Tables[0]);
		if (tqmts29.Tables[0].Rows.get_Count() == 0)
		{
			return 0;
		}
		tqmts0x["ST_NO"] = tqmts29.Tables[0].Rows[0]["ST_NO"].ToString();
		tqmts0x.Query("ST_NO");
		int count = 0;
		for (size_t i = 0; i < tqmts29.Tables[0].Rows.get_Count(); i++)
		{
			if (epex.SetValue("tqmtqb0", "ST_NO", 0, tqmts29.Tables[0].Rows[i]["ST_NO"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmtqb0", "PONO", 0, tqmts29.Tables[0].Rows[i]["PONO"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmtqq0", "ELM_CODE", i, tqmts29.Tables[0].Rows[i]["ELM_CODE"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmtqq0", "ELM_NAME", i, tqmts29.Tables[0].Rows[i]["ELM_NAME"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmtqq0", "ELM_VALUE", i, tqmts29.Tables[0].Rows[i]["ELM_ACT"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmtqq0", "ELM_ACCU", i, 1) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			count++;
		}
		sqlstr = " SELECT CODE AS ELM_CODE, CODE_DESC_1_CONTENT AS ELM_NAME, 0 AS ELM_ACT FROM tep0002  WHERE CODE_CLASS = 'QMYS2N' AND CODE IN("
			"			SELECT CODE  FROM tep0002  WHERE CODE_CLASS = 'QMYS2N' AND CODE IN('001',"
			"			'002',"
			"			'003',"
			"			'004',"
			"			'005',"
			"			'006',"
			"			'007',"
			"			'008',"
			"			'009',"
			"			'010',"
			"			'011',"
			"			'012',"
			"			'013',"
			"			'014',"
			"			'015',"
			"			'016',"
			"			'017',"
			"			'018',"
			"			'019',"
			"			'020',"
			"			'021',"
			"			'022',"
			"			'023',"
			"			'024',"
			"			'025',"
			"			'026',"
			"			'029',"
			"			'030',"
			"			'031',"
			"			'032',"			
			"			'033',"
			"			'035',"
			"			'036') MINUS"
			"			(SELECT elm_code FROM TQMTS29 WHERE HEAT_NO = @heat_no)) ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		tqmts29.Tables[0].Clear();
		cmd.ExecuteQuery(tqmts29.Tables[0]);

		for (size_t i = 0; i < tqmts29.Tables[0].Rows.get_Count(); i++)
		{
			if (epex.SetValue("tqmtqq0", "ELM_CODE", count, tqmts29.Tables[0].Rows[i]["ELM_CODE"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmtqq0", "ELM_NAME", count, tqmts29.Tables[0].Rows[i]["ELM_NAME"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmtqq0", "ELM_VALUE", count, tqmts29.Tables[0].Rows[i]["ELM_ACT"].ToString()) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (epex.SetValue("tqmtqq0", "ELM_ACCU", count, 1) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			count++;
		}
		if (tqmts0x["C_DIV"].ToString() == "1")
		{
			CDecimal wt_scarp_rc = 0;//1,2
			CDecimal wt_scarp_rcs = 0;//2,3
			CDecimal wt_scarp_frz = 0;//1,4,3
			CDecimal wt_alloy = 0;
			sqlstr = " SELECT sum(DEVO_WT) FROM TMMSMGY08 WHERE HEAT_NO = '" + heat_no + "' AND MAT_CODE IN (select MAT_CODE from tmmsm50 t WHERE CS_FLAG IN ('1','3','5','6')) ";
			Log::Trace("", "", "sqlstr ={0}", sqlstr);
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteReader();
			if (cmd.Read())
			{
				wt_scarp_rc = cmd.GetDecimal(1);
			}
			cmd.Close();
			sqlstr = " SELECT sum(DEVO_WT) FROM TMMSMGY08 WHERE HEAT_NO = '" + heat_no + "' AND MAT_CODE IN (select MAT_CODE from tmmsm50 t WHERE CS_FLAG IN ('2','3','5','7')) ";
			Log::Trace("", "", "sqlstr ={0}", sqlstr);
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteReader();
			if (cmd.Read())
			{
				wt_scarp_rcs = cmd.GetDecimal(1);
			}
			cmd.Close();
			sqlstr = " SELECT sum(DEVO_WT) FROM TMMSMGY08 WHERE HEAT_NO = '" + heat_no + "' AND MAT_CODE IN (select MAT_CODE from tmmsm50 t WHERE CS_FLAG IN ('1','3','4','7')) ";
			Log::Trace("", "", "sqlstr ={0}", sqlstr);
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteReader();
			if (cmd.Read())
			{
				wt_scarp_frz = cmd.GetDecimal(1);
			}
			cmd.Close();
			sqlstr = "SELECT sum(DEVO_WT)  FROM TMMSMGY08  WHERE HEAT_NO  ='"+ heat_no +"' AND  MAT_CODE IN (select MAT_CODE  from tmmsm50 t where MAT_TYPE in ('1','2','4')) ";//李振 周丽20241121
			Log::Trace("", "", "sqlstr ={0}", sqlstr);
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteReader();
			if (cmd.Read())
			{
				wt_alloy = cmd.GetDecimal(1);
			}
			cmd.Close();
			Log::Trace("", "", "合金重量 ={0}", wt_alloy);

			Log::Trace("", "", "废钢重量 ={0}", wt_scarp_rc,wt_scarp_rcs);
			if (wt_alloy > 0)
			{
				tqmtsfgb["REC_CREATE_TIME"] = datetime;
				tqmtsfgb["REC_CREATOR"] = s.userid;
				tqmtsfgb["HEAT_NO"] = heat_no;
				tqmtsfgb["SEQ_NO"] = Db::QueryCString("SELECT NVL(MAX(SEQ_NO),0)+1 FROM TQMTSFGB WHERE HEAT_NO='" + heat_no + "'");
				tqmtsfgb.Insert();
				if (wt_scarp_rc > 0)
				{
					if (epex.SetValue("tqmtqq0", "ELM_CODE", count, "999") < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("tqmtqq0", "ELM_NAME", count, "FGB") < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					CDecimal fgb = wt_scarp_rc * 100 / wt_alloy;
					if (epex.SetValue("tqmtqq0", "ELM_VALUE", count, fgb.Round(2)) < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("tqmtqq0", "ELM_ACCU", count, 1) < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					count++;
					tqmtsfgb["WT_SCARP_RC"] = fgb;
					tqmtsfgb.Update("WT_SCARP_RC", "HEAT_NO,SEQ_NO");
				}

				if (wt_scarp_rcs > 0)
				{
					if (epex.SetValue("tqmtqq0", "ELM_CODE", count, "998") < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("tqmtqq0", "ELM_NAME", count, "FGB") < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					CDecimal fgb = wt_scarp_rcs * 100 / wt_alloy;
					if (epex.SetValue("tqmtqq0", "ELM_VALUE", count, fgb.Round(2)) < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("tqmtqq0", "ELM_ACCU", count, 1) < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					count++;
					tqmtsfgb["WT_SCARP_RCS"] = fgb;
					tqmtsfgb.Update("WT_SCARP_RCS", "HEAT_NO,SEQ_NO");
				}
				if (wt_scarp_frz > 0)
				{
					if (epex.SetValue("tqmtqq0", "ELM_CODE", count, "997") < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("tqmtqq0", "ELM_NAME", count, "FGB") < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}

					CDecimal fgb = wt_scarp_frz * 100 / wt_alloy;
					if (epex.SetValue("tqmtqq0", "ELM_VALUE", count, fgb.Round(2)) < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("tqmtqq0", "ELM_ACCU", count, 1) < 0)
					{
						sprintf(s.msg, (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					tqmtsfgb["WT_SCARP_FRZ"] = fgb;
					tqmtsfgb.Update("WT_SCARP_FRZ", "HEAT_NO,SEQ_NO");
				}
			}
		}
		//不锈钢发送 废钢比到四级
		if (epex.SendTele() < 0)
		{
			Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
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


