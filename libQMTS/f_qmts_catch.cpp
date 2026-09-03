/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-16 19:54:28
Description: 制造标准读取工艺卡
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT
int f_qmts_elm_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//成分复制
int common_deal(CModel &tqmts, CModel &hqmts, bool exist, CString datetime)
{
	tqmts["CATCH_TIME"] = datetime;
	tqmts["CATCH_RESP"] = s.userid;
	if (exist == false)
	{
		tqmts["REC_CREATOR"] = s.userid;
		tqmts["REC_REVISOR"] = " ";
		tqmts["REC_CREATE_TIME"] = datetime;
		tqmts["REC_REVISE_TIME"] = " ";
		tqmts["VERSION"] = 0;
		tqmts["VALID_FLAG"] = "0";
		tqmts.Insert();
	}
	else
	{
		tqmts["REC_CREATOR"] = hqmts["REC_CREATOR"];
		tqmts["REC_REVISOR"] = s.userid;
		tqmts["DU_MAKER"] = s.userid;
		tqmts["REC_CREATE_TIME"] = hqmts["REC_CREATE_TIME"];
		tqmts["REC_REVISE_TIME"] = datetime;
		tqmts["DU_TIME"] = datetime;
		tqmts["DU_FLAG"] = "U";
		hqmts["DU_TIME"] = datetime;
		hqmts["DU_FLAG"] = "U";
		hqmts["DU_MAKER"] = s.userid;
		hqmts.Insert();
		tqmts["VERSION"] = tqmts["VERSION"].ToDecimal() + 1;
		tqmts["VALID_FLAG"] = "0";
		tqmts.Update("*", "ST_NO,FACTORY_DIV");
	}
}//通用处理
int f_qmts_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CString table_name = bcls_rec->Tables[1].Rows[0]["TABLE_NAME"];
		CModel tqmts("T" + table_name);
		CModel hqmts("H" + table_name);
		CModel tqmts0x("TQMTS0X");
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts0x["ST_NO"] = bcls_rec->Tables[0].Rows[i]["ST_NO"];
			tqmts0x["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[i]["FACTORY_DIV"];
			bool st_exist = tqmts0x.Query("ST_NO,FACTORY_DIV");
			if (st_exist == FALSE)
			{
				CFormattable arguments[] = { tqmts0x["ST_NO"].ToString() }; // 定义参数列表的数组
				CMessageFormat::Format(s.msg, "出钢记号[{0}]的工艺卡不存在。", arguments, 1); //格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (table_name == "QMTS08")
			{
				tqmts["ST_NO"] = tqmts0x["ST_NO"];
				tqmts["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				bool exist = tqmts.Query("ST_NO,FACTORY_DIV");
				if (exist)
				{
					hqmts.CopyFrom(tqmts);
				}
				/*处理TQMTS08 和tqmts0x 中共有字段，可以用CopyFrom,但是建议挨个字段对照，避免错误*/
				tqmts["LIQUID_TEMP"] = tqmts0x["LIQUID_TEMP"];
				/*处理共有字段结束*/
				doFlag = common_deal(tqmts, hqmts,exist, datetime);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_name == "QMTS03")
			{
				tqmts["ST_NO"] = tqmts0x["ST_NO"];
				tqmts["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				bool exist = tqmts.Query("ST_NO,FACTORY_DIV");
				if (exist)
				{
					hqmts.CopyFrom(tqmts);
				}
				/*处理TQMTS08 和tqmts0x 中共有字段，可以用CopyFrom,但是建议挨个字段对照，避免错误*/
				tqmts.CopyFrom(tqmts0x);
				/*处理共有字段结束*/
				doFlag = common_deal(tqmts, hqmts, exist, datetime);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_name == "QMTS04")
			{
				tqmts["ST_NO"] = tqmts0x["ST_NO"];
				tqmts["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				bool exist = tqmts.Query("ST_NO,FACTORY_DIV");
				if (exist)
				{
					hqmts.CopyFrom(tqmts);
				}
				/*处理TQMTS04 和tqmts0x 中共有字段，可以用CopyFrom,但是建议挨个字段对照，避免错误*/
				tqmts.CopyFrom(tqmts0x);
				/*处理共有字段结束*/
				doFlag = common_deal(tqmts, hqmts, exist, datetime);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_name == "QMTS06")
			{
				tqmts["ST_NO"] = tqmts0x["ST_NO"];
				tqmts["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				bool exist = tqmts.Query("ST_NO,FACTORY_DIV");
				if (exist)
				{
					hqmts.CopyFrom(tqmts);
				}
				/*处理TQMTS06 和tqmts0x 中共有字段，可以用CopyFrom,但是建议挨个字段对照，避免错误*/
				tqmts.CopyFrom(tqmts0x);
				/*处理共有字段结束*/
				doFlag = common_deal(tqmts, hqmts, exist, datetime);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_name == "QMTS07")
			{
				tqmts["ST_NO"] = tqmts0x["ST_NO"];
				tqmts["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				bool exist = tqmts.Query("ST_NO,FACTORY_DIV");
				if (exist)
				{
					hqmts.CopyFrom(tqmts);
				}
				/*处理TQMTS07 和tqmts0x 中共有字段，可以用CopyFrom,但是建议挨个字段对照，避免错误*/
				tqmts.CopyFrom(tqmts0x);
				/*处理共有字段结束*/
				doFlag = common_deal(tqmts, hqmts, exist, datetime);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (table_name == "QMTS0A")
			{
				tqmts["ST_NO"] = tqmts0x["ST_NO"];
				tqmts["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
				bool exist = tqmts.Query("ST_NO,FACTORY_DIV");
				if (exist)
				{
					hqmts.CopyFrom(tqmts);
				}
				/*处理TQMTS07 和tqmts0x 中共有字段，可以用CopyFrom,但是建议挨个字段对照，避免错误*/
				tqmts.CopyFrom(tqmts0x);
				/*处理共有字段结束*/
				doFlag = common_deal(tqmts, hqmts, exist, datetime);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		//复制成分
		doFlag = f_qmts_elm_catch(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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


