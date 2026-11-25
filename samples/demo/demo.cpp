#include <cstring>
#include <iostream>
#include <string>

#include "pdfsolid_conversion.h"

using namespace pdfsolid;
using namespace pdfsolid::base;
using namespace pdfsolid::common;
using namespace pdfsolid::conversion;

std::string input_file = "..\\input_files\\";
std::string output_file = "..\\output_files\\";
std::string password = "";

// A license key string, or the path of a license.xml file.
const char* license = "..\\license.xml";

// Size of the optional Word to PDF diagnostics buffer.
const int WORD_TO_PDF_DIAGNOSTICS_SIZE = 8192;

static void print_result(const char* name, ErrorCode error_code)
{
    std::cout << name << ": " << (int)error_code << std::endl;
}

int main()
{
    print_result("pdfsolid conversion sdk license verify", LibraryManager::LicenseVerify(license, "", ""));
    LibraryManager::Initialize("..\\..\\resource");
    LibraryManager::SetLogger(false, false);
    print_result("set document ai model", LibraryManager::SetDocumentAIModel("..\\..\\resource\\models\\documentai.model"));

    // Zero-initialize so the optional AI hooks (ocr, layout, table, ...) stay null.
    CConvertCallback callback = {};
    callback.progress = [](int current_page_count, int total_page_count) {
        std::cout << "convert rate: " << current_page_count << "/" << total_page_count << std::endl;
    };
    callback.cancel = []() -> bool {
        // Return true to cancel the conversion, false to continue.
        return false;
    };

    ConvertOptions opt;
    // pdf to word
    print_result("pdf to word", CPDFConversion::StartPDFToWord((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).append("word.docx").c_str(), opt, &callback));

    // pdf to excel
    print_result("pdf to excel", CPDFConversion::StartPDFToExcel((input_file + "excel.pdf").c_str(), password.c_str(), std::string(output_file).append("excel.xlsx").c_str(), opt, &callback));

    // pdf to ppt
    print_result("pdf to ppt", CPDFConversion::StartPDFToPpt((input_file + "powerpoint.pdf").c_str(), password.c_str(), std::string(output_file).append("powerpoint.pptx").c_str(), opt, &callback));

    // pdf to csv
    opt.excel_csv_format = true;
    print_result("pdf to csv", CPDFConversion::StartPDFToExcel((input_file + "excel.pdf").c_str(), password.c_str(), std::string(output_file).c_str(), opt, &callback));
    opt.excel_csv_format = false;

    // pdf to html
    opt.page_layout_mode = PageLayoutMode::e_Box;
    print_result("pdf to html", CPDFConversion::StartPDFToHtml((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).append("html.html").c_str(), opt, &callback));

    // pdf to rtf
    print_result("pdf to rtf", CPDFConversion::StartPDFToRtf((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).append("rtf.rtf").c_str(), opt, &callback));

    // pdf to image
    print_result("pdf to image", CPDFConversion::StartPDFToImage((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).c_str(), opt, &callback));

    // pdf to txt
    print_result("pdf to txt", CPDFConversion::StartPDFToTxt((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).append("txt.txt").c_str(), opt, &callback));

    // pdf to json
    print_result("pdf to json", CPDFConversion::StartPDFToJson((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).append("json.json").c_str(), opt, &callback));

    // pdf to markdown
    print_result("pdf to markdown", CPDFConversion::StartPDFToMarkdown((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).append("markdown.md").c_str(), opt, &callback));

    // pdf to searchable pdf
    opt.enable_ocr = true;
    opt.transparent_text = true;
    opt.language_count = 1;
    opt.languages[0] = OCRLanguage::e_English;
    print_result("pdf to searchable pdf", CPDFConversion::StartPDFToSearchablePdf((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).append("pdf.pdf").c_str(), opt, &callback));

    // pdf to ofd
    print_result("pdf to ofd", CPDFConversion::StartPDFToOfd((input_file + "word.pdf").c_str(), password.c_str(), std::string(output_file).append("pdf.ofd").c_str(), opt, &callback));

    // word to pdf with default options
    print_result("word to pdf", CPDFConversion::StartWordToPdf((input_file + "word.docx").c_str(), std::string(output_file).append("word_to_pdf.pdf").c_str(), &callback));

    // word to pdf with heading bookmarks, a page range and diagnostics
    // Word to PDF reads only page_ranges and word_pdf_bookmark_mode; other fields are ignored.
    ConvertOptions word_opt;
    word_opt.word_pdf_bookmark_mode = WordPdfBookmarkMode::e_Headings;
    strncpy_s(word_opt.page_ranges, sizeof(word_opt.page_ranges), "1-2", _TRUNCATE);
    char diagnostics[WORD_TO_PDF_DIAGNOSTICS_SIZE] = { 0 };
    ErrorCode word_result = CPDFConversion::StartWordToPdf((input_file + "word.docx").c_str(), std::string(output_file).append("word_to_pdf_bookmarks.pdf").c_str(), word_opt, &callback, diagnostics);
    print_result("word to pdf with options", word_result);
    // e_ErrUnsupportedFeature means the PDF is usually still produced, but some
    // features may affect fidelity; the diagnostics text lists them.
    if (diagnostics[0] != '\0') {
        std::cout << "word to pdf diagnostics:" << std::endl << diagnostics << std::endl;
    }

    char version[30] = { 0 };
    LibraryManager::GetVersion(version);
    std::cout << "pdfsolid conversion sdk version: " << version << std::endl;

    LibraryManager::ReleaseDocumentAIModel();
    LibraryManager::Release();
    return 0;
}
