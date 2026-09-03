/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2024-3-11 15:08:16
Description: 【二炼北】检查结果接收
**************************************************/

//框架头文件
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"


BM2F_ENTERACE_TELE(cm_210003_rcv)
int f_002103_snd(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn);//检验批发送
int f_cm_210003_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int   blkNum;
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_sql(conn);
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CModel tqmts2103("TQMTS2103");
	CModel tqmts2131("TQMTS2131");
	CModel tqmts2132("TQMTS2132");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		blkNum = bcls_rec->Tables.IndexOf("QMTSJY");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("QMTSJY");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("TABLE_NAME"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "TABLE_NAME");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("TABLE_NAME1"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "TABLE_NAME1");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("TABLE_NAME2"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "TABLE_NAME2");
		}

		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("PRIMARY_KEY"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "PRIMARY_KEY");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("PRIMARY_DATA"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "PRIMARY_DATA");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("MSGTYPE"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "MSGTYPE");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL1"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "FREESEL1");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL2"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "FREESEL2");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL3"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "FREESEL3");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL4"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "FREESEL4");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("FREESEL5"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "FREESEL5");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("IN_NAME"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "IN_NAME");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("IN_ROUT1"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "IN_ROUT1");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("IN_ROUT2"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "IN_ROUT2");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("OUT_NAME"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "OUT_NAME");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("OUT_ROUT1"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "OUT_ROUT1");
		}
		if (!bcls_rec->Tables["QMTSJY"].Columns.Contains("OUT_ROUT2"))
		{
			bcls_rec->Tables["QMTSJY"].Columns.Add(DT_STRING, "OUT_ROUT2");
		}

		tqmts2103.MergeFrom(bcls_rec->Tables["bapiheader"].Rows[0]);

		tqmts2103.MergeFrom(bcls_rec->Tables["input"].Rows[0]);


		for (int i = 0; i < bcls_rec->Tables["zchi_qm_qaimr"].Rows.get_Count(); i++)
		{
			tqmts2103.MergeFrom(bcls_rec->Tables["zchi_qm_qaimr"].Rows[i]);
			tqmts2103.TrimOrBlank();
			tqmts2103.Insert();

			for (int i = 0; i < bcls_rec->Tables["zchi_qm_qaise"].Rows.get_Count(); i++)
			{
				tqmts2131.MergeFrom(bcls_rec->Tables["zchi_qm_qaise"].Rows[i]);
				tqmts2131.TrimOrBlank();
				tqmts2131.Insert();
			}
			for (int i = 0; i < bcls_rec->Tables["zchi_qm_qierr"].Rows.get_Count(); i++)
			{
				tqmts2132.MergeFrom(bcls_rec->Tables["zchi_qm_qierr"].Rows[i]);
				tqmts2132.TrimOrBlank();
				tqmts2132.Insert();

				//接收就发送检验批发送（北）
				bcls_rec->Tables["QMTSJY"].Rows.Add();
				bcls_rec->Tables["QMTSJY"].Rows[0]["MSGTYPE"] = tqmts2103["msgtype"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL1"] = tqmts2103["freeuse1"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL2"] = tqmts2103["freeuse2"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL3"] = tqmts2103["freeuse3"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL4"] = tqmts2103["freeuse4"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["FREESEL5"] = tqmts2103["freeuse5"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["IN_NAME"] = tqmts2103["mes_message_key1"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["IN_ROUT1"] = tqmts2103["mes_message_key2"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["IN_ROUT2"] = tqmts2103["mes_message_key3"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["OUT_NAME"] = tqmts2103["mes_message_key4"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["OUT_ROUT1"] = tqmts2103["subsys"];
				bcls_rec->Tables["QMTSJY"].Rows[0]["OUT_ROUT2"] = "";
				

				bcls_rec->Tables["QMTSJY"].Rows[0]["TABLE_NAME"] = "TQMTS2103";
				bcls_rec->Tables["QMTSJY"].Rows[0]["TABLE_NAME1"] = "TQMTS2131";
				bcls_rec->Tables["QMTSJY"].Rows[0]["TABLE_NAME2"] = "TQMTS2132";

				bcls_rec->Tables["QMTSJY"].Rows[0]["PRIMARY_KEY"] = "RUECKMELNR";
				bcls_rec->Tables["QMTSJY"].Rows[0]["PRIMARY_DATA"] = tqmts2132["RUECKMELNR"];

				doFlag = f_002103_snd(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					Log::Trace("", __FUNCTION__, "-------调用f_002103_snd失败-------");
					throw CApplicationException(-1, s.msg, log.Location);
				}
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


