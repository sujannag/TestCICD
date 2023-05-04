
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <vector>

/* Include the C++ DDS API. */
#include "dds/dds.hpp"

/* Include data type and specific traits to be used with the C++ DDS API. */
#include "dds_demo.hpp"

using namespace org::eclipse::cyclonedds;

void write_blob_to_image(const std::vector<uint8_t>& image_data, const std::string& image_file) {
    std::cout << "=== [Subscriber] write_blob_to_image." << image_data.size() << std::endl;
    std::ofstream output_file(image_file, std::ios::binary);
    output_file.write(reinterpret_cast<const char*>(image_data.data()), image_data.size());
    output_file.close();
}

void write_blob_to_json(const std::vector<uint8_t>& json_data, const std::string& json_file) {
    std::cout << "=== [Subscriber] write_blob_to_json." << std::endl;
    std::ofstream output_file(json_file, std::ios::binary);
    output_file.write(reinterpret_cast<const char*>(json_data.data()), json_data.size());
    output_file.close();
}

int main() {
    try {
        std::cout << "=== [Subscriber] Create reader." << std::endl;

        /* First, a domain participant is needed.
         * Create one on the default domain. */
        dds::domain::DomainParticipant participant(domain::default_id());

        /* To subscribe to something, a topic is needed. */
        dds::topic::Topic<ImageTransfer::ImageAndMetadata> topic(participant, "ImageAndMetaDataTopic");

        /* A reader also needs a subscriber. */
        dds::sub::Subscriber subscriber(participant);

        /* Now, the reader can be created to subscribe to a HelloWorld message. */
        dds::sub::DataReader<ImageTransfer::ImageAndMetadata> reader(subscriber, topic);

        /* Poll until a message has been read.
         * It isn't really recommended to do this kind wait in a polling loop.
         * It's done here just to illustrate the easiest way to get data.
         * Please take a look at Listeners and WaitSets for much better
         * solutions, albeit somewhat more elaborate ones. */
        std::cout << "=== [Subscriber] Wait for message." << std::endl;
        bool poll = true;
        while (poll) {
            /* For this example, the reader will return a set of messages (aka
             * Samples). There are other ways of getting samples from reader.
             * See the various read() and take() functions that are present. */
            dds::sub::LoanedSamples<ImageTransfer::ImageAndMetadata> samples;

            /* Try taking samples from the reader. */
            samples = reader.take();
            // std::cout<<"I am in a loop" << std::endl;

            /* Are samples read? */
            if (samples.length() > 0) {
                /* Use an iterator to run over the set of samples. */
                dds::sub::LoanedSamples<ImageTransfer::ImageAndMetadata>::const_iterator sample_iter;
                for (sample_iter = samples.begin();
                     sample_iter < samples.end();
                     ++sample_iter) {
                    /* Get the message and sample information. */
                    const ImageTransfer::ImageAndMetadata& image_and_metadata = sample_iter->data();
                    const dds::sub::SampleInfo& info = sample_iter->info();

                    /* Sometimes a sample is read, only to indicate a data
                     * state change (which can be found in the info). If
                     * that's the case, only the key value of the sample
                     * is set. The other data parts are not.
                     * Check if this sample has valid data. */
                    if (info.valid()) {
                        std::cout << "=== [Subscriber] Message received:" << std::endl;
                        
                        write_blob_to_image(image_and_metadata.image_data(), "../output/image.jpg");
                        write_blob_to_json(image_and_metadata.json_metadata(), "../output/metadata.json");

                        /* Only 1 message is expected in this example. */
                        poll = false;
                    }
                }
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        }
    } catch (const dds::core::Exception& e) {
        std::cerr << "=== [Subscriber] DDS exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "=== [Subscriber] C++ exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "=== [Subscriber] Done." << std::endl;

    return EXIT_SUCCESS;
}
