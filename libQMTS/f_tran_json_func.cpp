/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      195315
Version:     1.0
Date:        2023-07-27 13:51:12
Description: json转换
**************************************************/

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cassert>
#include <curl/curl.h>
#include "json/json.h"

BM2_FUNCTION_EXPORT

void xxxcrypt(const char * user, const char * pass, char *cpasswd);
std::string codepage_test_sql = BM2GetCodePage();

typedef struct TRACE_DEBUG_INFO_
{
	std::string curlinfo_header_in;
	std::string curlinfo_header_out;
	std::string curlinfo_data_in;
	std::string curlinfo_data_out;
}trace_debug_info;

size_t curl_write_function_inq_ijud_test_sql1(void *ptr, size_t size, size_t nmemb, void *stream)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)stream);
	if (NULL == str || NULL == ptr)
	{
		return -1;
	}

	char* pData = (char*)ptr;
	str->append(pData, size * nmemb);
	return nmemb;
}

int curl_trace_inq_ijud_test_sql(CURL *handle, curl_infotype type,
	char *data, size_t size,
	void *userp)
{
	trace_debug_info * the_trace_debug_info = (trace_debug_info *)userp;
	const char *text;
	(void)handle; /* prevent compiler warning */

	switch (type) {
	case CURLINFO_TEXT:
		fprintf(stderr, "== Info: %s", data);
	default: /* in case a new one is introduced to shock us */
		return 0;

	case CURLINFO_HEADER_OUT:
		text = "=> Send header";
		the_trace_debug_info->curlinfo_header_out += std::string(data, size);
		break;
	case CURLINFO_DATA_OUT:
		text = "=> Send data";
		the_trace_debug_info->curlinfo_data_out += std::string(data, size);
		break;
	case CURLINFO_SSL_DATA_OUT:
		text = "=> Send SSL data";
		break;
	case CURLINFO_HEADER_IN:
		text = "<= Recv header";
		the_trace_debug_info->curlinfo_header_in += std::string(data, size);
		break;
	case CURLINFO_DATA_IN:
		text = "<= Recv data";
		the_trace_debug_info->curlinfo_data_in += std::string(data, size);
		break;
	case CURLINFO_SSL_DATA_IN:
		text = "<= Recv SSL data";
		break;
	}

	return 0;
}

/* **************************************************
登录接口：	调用REST服务。
接口路由地址 ：authenticate
传入参数：
"SysInfo":{
"Flag":0,           //服务返回状态，必传字段
"Msg":"",           //服务返回msg，必传字段
"SvcName":"",       //服务名，必传字段
"Sender":"",        //用户工号，必传字段
"UUID":"",          //可以为空
"UserName":"",      //用户中文名， 可以为空
"CompanyCode":"",   //账套，可以为空
"CompanyName":"",   //账套中文名，可以为空
"ForeIP": "",       //请求IP ,可以为空，
"ForeMac": "",      //请求Mac地址，可以为空
"ForeMachine": ""   //请求机器名，可以为空
},
返回值	：	大于等于0，完成了对REST服务的调用； 小于0， 未能完成对REST服务的调用。
************************************************** */

/* **************************************************
函数介绍：	规则执行接口
接口路由地址 ：director/exec
传入参数：EIInfo
表1 SYS_INFO:根据 机组... 规则引擎获取对应的项目代码
表2 DATA_CUSTOM:根据 模板实例名...规则引擎抓取大数据平台高频数据
表3 PROJECT_CONFIG 目标参数(正公差,负公差...) 提供给规则引擎 相应的标准值
返回值	：	大于等于0，完成了对REST服务的调用； 小于0， 未能完成对REST服务的调用。
************************************************** */

int f_tran_json_func(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	char security_pass[100] = "";
	std::string output_data;
	std::string output_data2;
	CDateTime start_time, end_time;
	trace_debug_info  the_trace_debug_info;
	CString request_id, response_id;
	CDbCommand  cmd(conn);

	CString sqlstr = "";
	CString siteUrl("");
	CString ename, password;
	CString tokenData;
	static std::string token;
	CString res_token, res_rule;
	CString tokenUrl = "";
	CString callUrl = "";
	CString ruleUrl;
	CString strValue;
	CString value;
	int time_max1;
	int time_max2;
	//第一张规则表的字段
	std::string projetct_ename, env_type, version;
	//第二张规则表的字段 不固定
	CString colname, type;
	DsType coltype;
	//第三张规则表的字段 不固定

	time_t timep;
	bool rememberme;
	EIClass inblk;
	int tsElaps_rest = 0;

	try
	{
		//EDLog(1, 1, "***********************************f_call_ijud_rest_svc-test begin************************************************");
		//获取token的条件
		rememberme = true;

		//*************************test***************************

		time_max1 = 180;
		time_max2 = 180;

		struct curl_slist *headers = NULL;
		// 开始
		ruleUrl = bcls_rec->Tables[0].Rows[0][0].ToString();
		callUrl = bcls_rec->Tables[0].Rows[0][1].ToString();
		value = bcls_rec->Tables[0].Rows[0][2].ToString();

		strValue = ruleUrl.ToUTF8(codepage_test_sql.c_str());
		EDLog(1, 1, "ruleUrl Info=%s", (const char*)ruleUrl);
		siteUrl = callUrl;
		EDLog(1, 1, "callUrl=%s", (const char*)callUrl);
		std::string input_data = (const char*)strValue;
		CURL * curl = curl_easy_init();
		CURLcode  curl_code;
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1)) != CURLE_OK)
		{
			EDLog(1, 1, "111");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}

		if ((curl_code = curl_easy_setopt(curl, CURLOPT_POST, 1)) != CURLE_OK)
		{
			EDLog(1, 1, "222");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}

		if ((curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L)) != CURLE_OK)
		{
			EDLog(1, 1, "333");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, curl_trace_inq_ijud_test_sql)) != CURLE_OK)
		{
			EDLog(1, 1, "444");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_easy_setopt(curl, CURLOPT_DEBUGDATA, &the_trace_debug_info)) != CURLE_OK)
		{
			EDLog(1, 1, "555");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_URL, (const char*)siteUrl)) != CURLE_OK)
		{
			EDLog(1, 1, "666");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input_data.c_str())) != CURLE_OK)
		{
			EDLog(1, 1, "777");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, input_data.length())) != CURLE_OK)
		{
			EDLog(1, 1, "888");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_write_function_inq_ijud_test_sql1)) != CURLE_OK)
		{
			EDLog(1, 1, "999");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &output_data2)) != CURLE_OK)
		{
			EDLog(1, 1, "000");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1)) != CURLE_OK)
		{
			EDLog(1, 1, "aaa");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		/*if ((curl_code = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5)) != CURLE_OK)*/
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30)) != CURLE_OK)
		{
			EDLog(1, 1, "bbb");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, time_max1)) != CURLE_OK)
		{
			EDLog(1, 1, "ccc");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false)) != CURLE_OK)
		{
			EDLog(1, 1, "ddd");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		if ((curl_code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false)) != CURLE_OK)
		{
			EDLog(1, 1, "eee");
			curl_easy_cleanup(curl);
			throw CApplicationException(curl_easy_strerror(curl_code));
		}

		headers = NULL;
		headers = curl_slist_append(headers, "Accept: *");
		headers = curl_slist_append(headers, "Content-Type: application/json;charset=UTF-8");
		if ((curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers)) != CURLE_OK)
		{
			Log::Trace("", "", "curl_code=[{0}] ,{1}", curl_code, (const char*)headers);
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers); /* free the header list */
			throw CApplicationException(curl_easy_strerror(curl_code));
		}

		start_time = CDateTime::Now();
		curl_code = curl_easy_perform(curl);
		end_time = CDateTime::Now();

		if (curl_code != CURLE_OK)
		{
			EDLog(1, 1, "curl_easy_perform error-json-");
			Log::Trace("", "", "curl_code=[{0}] ", curl_code);
			curl_easy_cleanup(curl);
			curl_slist_free_all(headers); /* free the header list */
			throw CApplicationException(curl_easy_strerror(curl_code));
		}
		curl_easy_cleanup(curl);
		curl_slist_free_all(headers); /* free the header list */
		end_time = CDateTime::Now();

		/*int count = 0;
		int iRow = 0;*/

		Json::Value eiinfo;
		Json::Reader reader;
		Log::Trace("", "", "666666");
		//char *c = new char[20];
		//strcpy(c, output_data2.c_str());
		//c_str()：生成一个const char*指针，指向以空字符终止的数组。
		//c_str()函数返回一个指向正规C字符串的指针, 内容与本string串相同
		//这个数组的数据是临时的，当有一个改变这些数据的成员函数被调用后，其中的数据就会失效。
		//因此要么现用先转换，要么把它的数据复制到用户自己可以管理的内存中
		//Log::Trace("", "", "数据：[{0}]", c);
		//Log::Trace("", "", "777777", output_data2.c_str());

		if (!reader.parse(output_data2.c_str(), eiinfo, false))
		{
			throw CApplicationException(reader.getFormattedErrorMessages());
		}
		Log::Trace("", "", "--value=[{0}]---", value);
		
		Json::Value jsonMeta = eiinfo[value];
	
		//CString meta = Json::FastWriter().write(jsonMeta);
		//if (sysflag < 0)
		//{
		//	//Log::Trace("", "", "--output_data2=[{0}]---", output_data2.c_str());
		//	//Log::Trace("", "", "--output_data2=[{0}]---", jsonDecoded);
		//	throw CApplicationException("eplat获取数据失败");
		//}
		//Log::Trace("", "", "--output_data2=[{0}]---", meta);

		int colcnt = jsonMeta.size();
		int count = 0;
		int iRow = 0;
		if (colcnt == 0)
		{
			Log::Trace("", "", "--sw value=[{0}]---", value);
			strcpy(s.msg, "返回履历列数为0！");
			return -1;
		}
		Log::Trace("", "", "--ll value=[{0}]---", value);
		bcls_ret->Tables[0].Clear();
		//
		//bcls_ret->Tables[0].Columns.Add(DT_STRING, "VALUE");
		//std::string column;
		std::string strvalue;
		CString keyval;
		CString val;

		//Log::Trace("", "", "--colcnt=[{0}]---", colcnt);
		for (int i = 0; i < colcnt; i++)b
		{
			//Log::Trace("", "", "--进行到这了---");
			Json::Value jsonData = jsonMeta[i];
			count = jsonData.size();
			//Log::Trace("", "", "--第[{0}]组数,共[{1}]列---", i, count);
			bcls_ret->Tables[0].Rows.Add();
			for (int n = 0; n < count; n++)
			{
				Json::Value::Members member = jsonData.getMemberNames();
				for (auto key : member)
				{
					keyval = CString::ToCurrentCodePage("UTF-8", key.c_str());
					strvalue = jsonData[key].asString();
					val = CString::ToCurrentCodePage("UTF-8", strvalue.c_str());
					//Log::Trace("", "", "--keyval=[{0}]-val=[{1}]---", keyval, val);
					if (!bcls_ret->Tables[0].Columns.Contains(keyval))
					{
						bcls_ret->Tables[0].Columns.Add(DT_STRING, keyval);
					}
					iRow = bcls_ret->Tables[0].Rows.get_Count();
					bcls_ret->Tables[0].Rows[iRow - 1][keyval] = val;
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