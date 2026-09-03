/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
*  	MMS 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_qmts_sample_no_snd
*  程序描述			: 炼钢试样号下发
*  备注说明			: 供计划调用[此函数从文丰项目组搬过来的，计划那边如何调用请项目组咨询文丰项目组的魏化媚，建议在项目组里订制]
*                     此函数的某些细节项目组需要修改： 如每道工序上样的个数，ONH是否分开生成，生成气体样的工序及条件。
                      电文号已导进产品化供参考。
					  由于外面项目的试样号规则和系统不一致，所以另外按用户要求拼了一个sample_no给二级系统用,只是发电文用未记录到表里。
					  MES系统里还是用的st_sample_no.
*  修改历史			:
*  		2017/08/15 	yiling			(ADD)程序建立
*			... ...
* **************************************************************************** */
/// <summary>
/// <para>数据库表： </para>
/// <para>数据库表： </para>
/// <para>数据库表： </para>
/// <para>主调用函数：供计划调用。QMTS25S画面备用按钮下发</para>
/// </summary>
/// <param name> </param>
/// <returns>成败标记</returns>
//框架公用头文件，勿删
#include "stdafx.h"

//程序用头文件
#include "epex.h" 




/*<remark>=========================================================
/// <summary>
/// 向检化验系统发送试样号电文
/// <para>1. qmts25s_snd主程序调用；</para>
/// 试样号规则：
///第一位：厂别，一炼钢  ‘A’
///第二位： 试样区分：(1：钢样，4：气体样)
///第三,四位：工位设备号：(B:转炉，C:连铸，I:模铸，V:VD，L：LF，B1，1#转炉等等
///第五位：工位顺序号
///第六位: 气体样区分（1：ONH样，钢样：Z）
///第七位：流水号

/// <para>3. 组织发送电文；</para>
/// <para>4. 电文发送。</para>
/// </summary>
/// <param name="。</param>
/// <returns>无</returns>
===========================================================</remark>*/
BM2_FUNCTION_EXPORT
int f_qmts_sample_no_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString sqlstr = "";
	CString s_whole_backlog = "";//全程工序，记录炼钢路径
	CString s_refine_route_code = "";//精炼路径
	CString s_dev_code = "";//铸机号

	CString v_st_sample_no = "";
	CString lpsz_tc_no = " ";
	CString  datetime = "";
	CString s_sg_sign = "";
	CString s_sg_std = " ";
	CString s_ic_cc_flag = "";
	CString s_sample_no = "";
	int snd_sum = 0;

	EPEX epex(&s, conn);
	int i_max = 0;//定义试样数量

	int v_count = 0;
	/* 实体类定义 */
	CModel tqmts24("TQMTS24");
	CModel tpssm12("TPSSM12");
	CModel tqmts0x("TQMTS0X");
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		lpsz_tc_no = "MEJY01";
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			/* 取得单行传入信息 */
			tqmts24.Reset();
			tqmts24.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			tqmts24["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts24["REC_CREATOR"] = s.userid;

			s_dev_code = bcls_rec->Tables[0].Rows[i]["dev_code"];

			//写临时规则，炼钢工位号暂时只传工序代码
			//只自动形成最大数量的转炉、氩站、LF、RH、连铸样，其他默认定位人工形成。
			Log::Trace("", "", "f_qmts_sample_no_snd:-- - heat_no[{0}]第[{1}]个", (const char*)tqmts24["HEAT_NO"].ToString());
			Log::Trace("", "", "f_qmts_sample_no_snd IN:---st_no = [{0}][{1}]", (const char*)tqmts24["ST_NO"].ToString());
			Log::Trace("", "", "f_qmts_sample_no_snd IN:---s_dev_code = [{0}][{1}", (const char*)s_dev_code);
			Log::Trace("", "", "f_qmts_sample_no_snd IN:---pono = [{0}][{1}", (const char*)tqmts24["PONO"].ToString());


			//校验传入参数
			if (tqmts24["HEAT_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, "熔炼号不允许为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//校验传入参数
			if (tqmts24["PONO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, "PONO不允许为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tqmts24["ST_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg, "出钢记号不允许为空。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//校验传入参数
			if (s_dev_code.GetLength() != 2)
			{
				strcpy(s.msg, "设备必须为2位。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.Initialize(lpsz_tc_no) < 0)
			{
				Log::Trace("", "", "f_qmts_sample_no_snd---Initialize出错[{0}]", (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts24["FACTORY_DIV"] = "A";
			tqmts24["WHOLE_BACKLOG_CODE"] = s_dev_code.Substring(0, 1);
			if (tqmts24["WHOLE_BACKLOG_CODE"].ToString() == "I")
			{
				return 0;
			}
			if (tqmts24["WHOLE_BACKLOG_CODE"].ToString() == "B"  || tqmts24["WHOLE_BACKLOG_CODE"].ToString() == "V" )
			{
				snd_sum = 3;
			}
			else if ( tqmts24["WHOLE_BACKLOG_CODE"].ToString() == "C")  ////update by yiling 20180102  尹春生要求连铸只生成1条样
			{
				snd_sum = 2;
			}
			else
			{
				snd_sum = 4;
			}
			switch (conn->DatabaseKind)
			{
				//转炉(电炉):3  精炼:4  连铸:5  决定:6
			case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:				// MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:						// 所有数据库适用，通用SQL语句
				sqlstr = "SELECT * FROM "
					"(SELECT * FROM TPSSM12 WHERE HEAT_NO = @heat_no and dev_code=@s_dev_code AND AREA_ID < 6 "
					" UNION "
					" SELECT * FROM TPSSM42 WHERE HEAT_NO = @heat_no and dev_code=@s_dev_code AND AREA_ID < 6 )"
					" ORDER BY CHARGE_NO ASC ";
				break;
			}

			Log::Trace("", "", "sqlstr=[{0}]", sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts24["HEAT_NO"].ToString());
			cmd_inq.Parameters.Set("s_dev_code", s_dev_code);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				cmd_inq.Fetch(tpssm12);
			}
			else
			{
				strcpy(s.msg, "炼钢计划表未查到数据");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			cmd_inq.Close();
			//获取牌号标准模连铸标记
			sqlstr = "  SELECT LABEL1,IC_CC_FLAG FROM TQMTS0X WHERE ST_NO = @ST_NO 	";
			Log::Trace("", "", "sqlstr=[{0}]", sqlstr);

			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("ST_NO", tqmts24["ST_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				s_sg_sign = cmd_inq.GetString(1);
				s_ic_cc_flag = cmd_inq.GetString(2);
			}
			cmd_inq.Close();
			for (int i = 1; i <= snd_sum; i++)
			{
				//拼试样号——炼钢厂(1)+炼钢试样区分(1)+工序(1)+工位(1)+工序顺序号+气体类型区分(1)+炼钢试样顺序号(1)  非气体样：气体类型区分写死"Z"  ///一个精炼设备，有可能追加LF工序，会有多条，加个charge_no号识别试样update by yiling 20161018
				////钢样
				tqmts24["ST_SAMPLE_DIV"] = "1";
				//tqmts24["ST_SAMPLE_NO"] = "A1" + tpssm12["DEV_CODE"].ToString().Trim() + tpssm12["CHARGE_NO"].ToDecimal().ToString() + "Z" + "1";
				tqmts24["ST_SAMPLE_NO"] = "A1" + tpssm12["DEV_CODE"].ToString().Trim() + tpssm12["CHARGE_NO"].ToDecimal().ToString() + "Z" + CDecimal(i).ToString();
				s_sample_no = tpssm12["DEV_CODE"].ToString().Trim() + tpssm12["CHARGE_NO"].ToDecimal().ToString() + tqmts24["ST_SAMPLE_NO"].ToString().Substring(1, 1)  + tqmts24["ST_SAMPLE_NO"].ToString().Substring(6, 1);
				//s_sample_no = tpssm12["DEV_CODE"].ToString().Trim() + tqmts24["ST_SAMPLE_NO"].ToString().Substring(1, 1) + tqmts24["ST_SAMPLE_NO"].ToString().Substring(6, 1);
				Log::Trace("", "", "s_sample_no[{0}]", s_sample_no);
				if (tpssm12["DEV_CODE"].ToString().Trim() == "" || tpssm12["CHARGE_NO"].ToDecimal() <= 0)
				{
					Log::Trace("", "", "tpssm12.DEV_CODE[{0}]tpssm12.CHARGE_NO[{1}]", tpssm12["DEV_CODE"].ToString(), tpssm12["CHARGE_NO"].ToDecimal());
					continue;
				}
				Log::Trace("", "", "ST_SAMPLE_NO[{0}]", tqmts24["ST_SAMPLE_NO"].ToString());
				if (tqmts24.QueryCount("FACTORY_DIV, PONO,ST_SAMPLE_NO,HEAT_NO"))
				{
					Log::Trace("", "", "查询到记录跳出循环");
					continue;
				}
				Log::Trace("", "", "发送钢样试样号[{0}]", tqmts24["ST_SAMPLE_NO"].ToString()); 
				tqmts24["ST_SAMPLE_SEQ"] = i;
				Log::Trace("", "", "ST_SAMPLE_SEQ[{0}]", tqmts24["ST_SAMPLE_SEQ"].ToDecimal());
				////写24表，将试样号保存
				//tqmts24.Delete("FACTORY_DIV, PONO,ST_SAMPLE_NO");  ///防止计划多次触发调用此函数
				tqmts24.Insert();
				/*if (epex.Initialize(lpsz_tc_no) < 0)
				{
					Log::Trace("", "", "f_qmts_sample_no_snd---Initialize出错[{0}]", (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}*/
				if (epex.SetValue(0, tqmts24) < 0)
				{
					Log::Trace("", "", "f_qmts_sample_no_snd1---SetValue[{0}]", (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("sg_std", 0, s_sg_std) < 0)       //标准
				{
					Log::Trace("", "", "f_qmts_sample_no_snd2---SendTele[%s]", (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("sg_sign", 0, s_sg_sign) < 0)      //牌号
				{
					Log::Trace("", "", "f_qmts_sample_no_snd3---SendTele[%s]", (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("ic_cc_flag", 0, s_ic_cc_flag) < 0)      //模连铸标记
				{
					Log::Trace("", "", "f_qmts_sample_no_snd4---SendTele[%s]", (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (epex.SetValue("sample_no", 0, s_sample_no) < 0)      //模连铸标记
				{
					Log::Trace("", "", "f_qmts_sample_no_snd5---SendTele[%s]", (const char*)epex.GetMsg());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//发送电文
				if (epex.SendTele() < 0)
				{
					Log::Trace("", "", "f_qmts_sample_no_snd5---SendTele[%s]", (const char*)epex.GetMsg());
					strcpy(s.msg, "发送试样电文失败");
					throw CApplicationException(-1, s.msg, log.Location);
				}
				// 释放
				//epex.Uninitialize();
			}
			Log::Trace("", "", "tqmts24.WHOLE_BACKLOG_CODE[{0}]", tqmts24["WHOLE_BACKLOG_CODE"].ToString());
			if (tqmts24["WHOLE_BACKLOG_CODE"].ToString() == "C" || tqmts24["WHOLE_BACKLOG_CODE"].ToString() == "V")  ///连铸和模铸上的VD发送生成气体样
			{
				if (tqmts24["WHOLE_BACKLOG_CODE"].ToString() == "V")
				{
					tqmts0x["ST_NO"] = tqmts24["ST_NO"];
					tqmts0x.Query("ST_NO");
					if (tqmts0x["IC_CC_FLAG"].ToString() !="I")
					{
						Log::Trace("", "", "VD不在模铸上");
						continue;
					}
				}
				Log::Trace("", "", "准备自动生成气体样");
				switch (conn->DatabaseKind)
				{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					if (tqmts24["ST_NO"].ToString().Substring(1, 1) == "0" || tqmts24["ST_NO"].ToString().Substring(1, 1) == "1") ////板坯只有需要判定的，才生成试样update by yiling 20180102  尹春生\崔晨瑞提。
					{
						sqlstr = "SELECT * FROM tqmts02 WHERE st_no=@tqmts24.ST_NO"
							" and ELM_NAME IN ('O','N','H')"
							" and SMELT_CHEMI_FLAG ='3'"
							;
					}
					else
					{
						sqlstr = "SELECT * FROM tqmts02 WHERE st_no=@tqmts24.ST_NO"
							" and ELM_NAME IN ('O','N','H')"
							;
					}

					break;  
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("tqmts24.ST_NO", tqmts24["ST_NO"].ToString());
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					tqmts24["ST_SAMPLE_DIV"] = "4";
					tqmts24["ST_SAMPLE_NO"] = "A4" + tpssm12["DEV_CODE"].ToString().Trim() + tpssm12["CHARGE_NO"].ToDecimal().ToString() + "1" + "1";
					Log::Trace("", "", "发送ON样试样号[{0}]", tqmts24["ST_SAMPLE_NO"].ToString());
					if (tqmts24.QueryCount("FACTORY_DIV, PONO,ST_SAMPLE_NO,HEAT_NO"))
					{
						Log::Trace("", "", "查询到记录跳出循环");
						continue;
					}
					////写24表，将试样号保存
					tqmts24["GAS_TYPE_DIV"] = "1";
					tqmts24["ST_SAMPLE_SEQ"] = 1;
					//自动生成的气体样默认为“保气体”
					tqmts24["ELM_TYPE_DIV"] = "1";
					//tqmts24.TEST_REMARK = "保气体";
					tqmts24.Delete("FACTORY_DIV, PONO,ST_SAMPLE_NO");  ///防止计划多次触发调用此函数
					tqmts24.Insert();
					/*if (epex.Initialize(lpsz_tc_no) < 0)
					{
						Log::Trace("", "", "f_qmts_sample_no_snd---Initialize出错[{0}]", (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}*/
					if (epex.SetValue(0, tqmts24) < 0)
					{
						Log::Trace("", "", "f_qmts_sample_no_snd---SetValue[{0}]", (const char*)epex.GetMsg());
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("sg_std", 0, s_sg_std) < 0)       //标准
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("sg_sign", 0, s_sg_sign) < 0)      //牌号
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					if (epex.SetValue("ic_cc_flag", 0, s_ic_cc_flag) < 0)      //模连铸标记
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					s_sample_no = tpssm12["DEV_CODE"].ToString().Trim() + tpssm12["CHARGE_NO"].ToDecimal().ToString() + tqmts24["ST_SAMPLE_NO"].ToString().Substring(1, 1) + tqmts24["ST_SAMPLE_NO"].ToString().Substring(6, 1);
					//s_sample_no = tpssm12["DEV_CODE"].ToString().Trim() + tqmts24["ST_SAMPLE_NO"].ToString().Substring(1, 1)  + tqmts24["ST_SAMPLE_NO"].ToString().Substring(6, 1);
					Log::Trace("", "", "s_sample_no[{0}]", s_sample_no);
					if (epex.SetValue("sample_no", 0, s_sample_no) < 0)      //模连铸标记
					{
						throw CApplicationException(-1, s.msg, log.Location);
					}
					//发送电文
					if (epex.SendTele() < 0)
					{
						Log::Trace("", "", "f_qmts_sample_no_snd---SendTele[%s]", (const char*)epex.GetMsg());

						strcpy(s.msg, "发送试样电文失败");
						throw CApplicationException(-1, s.msg, log.Location);
					}
					// 释放
					//epex.Uninitialize();
				}
				cmd_inq.Close();
			}
			epex.Uninitialize();
			doFlag = 0;
			s.sqlcode = 0;
			(void)strcpy(s.msg, _RES("GCRSS0000002")/*处理成功。*/);
		}


	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000021")/*信息读取失败。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1; //数据库异常时返回-1，事务将被回滚
	}
	catch (const CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return(doFlag);
}


