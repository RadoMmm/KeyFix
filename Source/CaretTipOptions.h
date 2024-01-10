#pragma once
#include <nlohmann/json.hpp>

struct CaretTipOptions
{
	bool ShowTip = false;
	COLORREF BkColor = RGB(255,255,0);
	COLORREF TextColor = RGB(0,0,0);
};

void to_json(nlohmann::json& j, const CaretTipOptions& p);
void from_json(const nlohmann::json& j, CaretTipOptions& p);

