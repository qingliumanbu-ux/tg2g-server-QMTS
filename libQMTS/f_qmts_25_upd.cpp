/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-28 19:54:28
Description: 成分判定
成分判定结果小代码QM1I
**************************************************/

#include "stdafx.h"
#include "math.h"

BM2_FUNCTION_EXPORT

int f_qmts_cf_accu(CString elm_code, CDecimal elm_act, CDecimal main_min, CDecimal main_max, CDecimal main_aim)//成分修约确定修约位数
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	int elm_accu = 0;//元素修约位数
	try
	{
		if (elm_code == "055" || elm_code == "064" || elm_code == "058" || elm_code == "052" || elm_code == "096")// Mn|Cu|Ni|Cr|Mo元素 	  
		{
			elm_accu = 2;
		}
		else if (elm_code == "014" || elm_code == "001" || elm_code == "016" || elm_code == "011" || elm_code == "042")// N|O|H|B|Ca元素
		{
			elm_accu = 4;
		}
		else if (elm_code == "030" || elm_code == "027" || elm_code == "211" || elm_code == "093" || elm_code == "051" || elm_code == "048" || elm_code == "091" || elm_code == "119")// P|Alt|Als|Nb|V|Ti|Zr|Sn元素
		{
			elm_accu = 3;
		}
		else if (elm_code == "012")// C元素
		{
			if (main_min < 0.01)
			{
				elm_accu = 4;
			}
			else if (main_min <= 0.1)
			{
				elm_accu = 3;
			}
			else
			{
				elm_accu = 2;
			}
		}
		else if (strcmp(elm_code, "028") == 0)// Si元素
		{
			if (main_max <= 0.1)
			{
				elm_accu = 3;
			}
			else if (main_min >= 0.08)
			{
				elm_accu = 2;
			}
			else
			{
				elm_accu = -1;
			}
		}
		else if (strcmp(elm_code, "032") == 0)// S元素
		{
			if (main_max >= 0.006)
			{
				elm_accu = 3;
			}
			else
			{
				elm_accu = 4;
			}
		}
		else//除此意外两位修约精度
		{
			elm_accu = 2;
		}
		return elm_accu;
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
}//修约位数

int f_qmts_cf_round(CDecimal &elm_act, int elm_accu)//成分修约
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		if (elm_accu > 0)
		{
			CDecimal v_elm_format = pow(10.0, elm_accu);
			elm_act = elm_act * v_elm_format;
			int z_value = floor(elm_act.ToDouble());//取传入长度的整数部分
			Log::Trace("", __FUNCTION__, " z_value [{0}] elm_act[{1}] ", z_value, elm_act);
			float x_value = (elm_act.ToDouble() - z_value);//取传入长度的小数部分
			Log::Trace("", __FUNCTION__, " x_value [{0}]  ", x_value);
			if (x_value == 0.5)
			{
				if (z_value % 2 == 0)
				{
					elm_act = z_value;
				}
				else
				{
					elm_act = z_value + 1;
				}
			}
			else
			{
				elm_act = floor(elm_act.ToDouble() + 0.5);
			}
			elm_act = elm_act.ToDouble() / v_elm_format.ToDouble();
			Log::Trace("", __FUNCTION__, " 修约后elm_act [{0}]  ", elm_act);
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
}//修约

int f_qmts_sample_flag(CModel &tqmts25, CModel &tqmts24, CDecimal elm_act, CDecimal main_min, CDecimal main_max)//是否取样
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts9ck("TQMTS9CK");
	try
	{
		tqmts9ck["ELM_CODE"] = tqmts25["ELM_CODE"];
		if (tqmts9ck.Query("ELM_CODE"))
		{
			if (tqmts9ck["PASS_FLAG"].ToString() == "1")//判断偏差值
			{
				if (elm_act< main_min - tqmts9ck["ELM_DIFF"] || elm_act> main_max + tqmts9ck["ELM_DIFF"])
				{
					tqmts25["SAMPLE_FLAG"] = "1";
					tqmts24["SAMPLE_FLAG"] = "1";
					tqmts25.Update("SAMPLE_FLAG");
					tqmts24.Update("SAMPLE_FLAG");
					return 1;
				}
			}
			else if (tqmts9ck["PASS_FLAG"].ToString() == "2")
			{
				if (elm_act< main_min * (1 - tqmts9ck["ELM_DIFF_PER"].ToDecimal()) || elm_act > main_max * (1 + tqmts9ck["ELM_DIFF_PER"].ToDecimal()))
				{
					tqmts24["SAMPLE_FLAG"] = "1";
					tqmts25["SAMPLE_FLAG"] = "1";
					tqmts25.Update("SAMPLE_FLAG");
					tqmts24.Update("SAMPLE_FLAG");
					return 1;
				}
			}
		}
		return 0;

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

int f_qmts_25_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	CModel tqmts25("TQMTS25");
	CModel tqmts24("TQMTS24");
	CModel tqmts0x("TQMTS0X");
	EIClass bcls_elm;//存标准和元素
	int elm_no = 0;//不合格元素个数
	try
	{
		CString table_name = bcls_rec->Tables[0].Rows[0]["TABLE_NAME"].ToString();
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		CString st_sample_no = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString();
		tqmts25["HEAT_NO"] = heat_no;
		tqmts25["ST_SAMPLE_NO"] = st_sample_no;
		tqmts24["HEAT_NO"] = heat_no;
		tqmts24["ST_SAMPLE_NO"] = st_sample_no;
		tqmts24.Query();
		tqmts0x["ST_NO"] = tqmts24["ST_NO"];
		tqmts0x.Query("ST_NO");
		//试样设置为未判定
		sqlstr = "UPDATE tqmts25 SET ELM_OK = '0' WHERE HEAT_NO = @heat_no AND st_sample_no = @st_sample_no";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.Parameters.Set("st_sample_no", st_sample_no);
		cmd.ExecuteNonQuery();
		cmd.Close();
		//判定结果.
		sqlstr = " SELECT t1.ELM_CODE, T1.ELM_NAME, T1.MAIN_MIN, T1.MAIN_MAX, T1.MAIN_AIM, T1.SPE_MIN, T1.SPE_MAX, T2.ELM_ACT, T2.ELM_ACT_OLD  FROM TQMTS02 T1"
			"			LEFT JOIN(SELECT * FROM TQMTS25 WHERE HEAT_NO = @heat_no AND ST_SAMPLE_NO = @st_sample_no) T2"
			"			ON T1.ELM_CODE = T2.ELM_CODE"
			"			WHERE T1.IDX_NO = (SELECT ELM_STD_IDX_A FROM TQMTS0X WHERE ST_NO = @st_no) ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.Parameters.Set("st_sample_no", st_sample_no);
		cmd.Parameters.Set("st_no", tqmts24["ST_NO"]);
		cmd.ExecuteQuery(bcls_elm.Tables[0]);
		for (int i = 0; i < bcls_elm.Tables[0].Rows.get_Count(); i++)
		{
			CDecimal main_min = bcls_elm.Tables[0].Rows[i]["MAIN_MIN"];
			CDecimal main_max = bcls_elm.Tables[0].Rows[i]["MAIN_MAX"];
			CDecimal main_aim = bcls_elm.Tables[0].Rows[i]["MAIN_AIM"];
			CDecimal spe_min = bcls_elm.Tables[0].Rows[i]["SPE_MIN"];
			CDecimal spe_max = bcls_elm.Tables[0].Rows[i]["SPE_MAX"];
			CDecimal elm_act = bcls_elm.Tables[0].Rows[i]["ELM_ACT_OLD"];
			tqmts25["ELM_CODE"] = bcls_elm.Tables[0].Rows[i]["ELM_CODE"];
			tqmts25["ELM_OK"] = "0";
			Log::Trace("", "", "ELM_NAME = [{0}],main_min=[{1}],main_max=[{2}],spe_min=[{3}],spe_max = [{4}],elm_act =[{5}]", bcls_elm.Tables[0].Rows[i]["ELM_NAME"].ToString(), main_min, main_max, spe_min, spe_max, elm_act);
			if (bcls_elm.Tables[0].Rows[i]["ELM_ACT_OLD"] != CDBNull(1))//有实绩成分
			{
				//气体样判断ONH
				if (tqmts24["ST_SAMPLE_DIV"].ToString() == "G")
				{
					if (tqmts25["ELM_CODE"].ToString() != "014" && tqmts25["ELM_CODE"].ToString() != "015"  && tqmts25["ELM_CODE"].ToString() != "016")
					{
						continue;
					}
				}
				if (elm_act >= spe_min && elm_act <= spe_max)//内控合格
				{
					tqmts25["ELM_OK"] = "8";
				}
				else//内控不合格
				{
					tqmts25["ELM_OK"] = "1";
					elm_no++;
					tqmts24["REMARK"] = tqmts24["REMARK"].ToString() + bcls_elm.Tables[0].Rows[i]["ELM_NAME"].ToString() + ",";
					//f_qmts_sample_flag(tqmts25, tqmts24, elm_act, main_min, main_max);
				}
				tqmts25["ELM_ACT"] = elm_act;
				tqmts25.Update("ELM_OK,ELM_ACT");
			}
			else//无实绩成分
			{
				Log::Trace("", "", "无成分实绩");
				if (st_sample_no.Substring(11, 1) == "T")//钢水样
				{
					elm_no++;
					tqmts24["REMARK"] = tqmts24["REMARK"].ToString() + bcls_elm.Tables[0].Rows[i]["ELM_NAME"].ToString() + ",";
				}
			}
		}
		cmd.Close();



		if (elm_no)
		{
			tqmts24["JUDGE_CODE"] = "2";
			tqmts24["REMARK"] = tqmts24["REMARK"].ToString() + "不合格";
		}
		else
		{
			tqmts24["JUDGE_CODE"] = "1";
			tqmts24["REMARK"] = " ";
		}
		tqmts24.Update("JUDGE_CODE,REMARK");

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


