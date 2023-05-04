
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

void read_image_to_blob(const std::string& image_file, std::vector<uint8_t>& image_data) {
    
    std::ifstream input_file(image_file, std::ios::binary | std::ios::ate);
    std::streamsize size = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    image_data.resize(size);
    input_file.read(reinterpret_cast<char*>(image_data.data()), size);
    std::cout << "=== [Publisher] read_image_to_blob." << image_data.size() << std::endl;
    input_file.close();

    // Mat image = imread(image_file, IMREAD_GRAYSCALE);
}

void read_json_to_blob(const std::string& json_file, std::vector<uint8_t>& json_data) {
    std::ifstream input_file(json_file, std::ios::binary | std::ios::ate);
    std::streamsize size = input_file.tellg();
    input_file.seekg(0, std::ios::beg);

    json_data.resize(size);
    input_file.read(reinterpret_cast<char*>(json_data.data()), size);
    input_file.close();
}

int main() {

    try {
        std::cout << "=== [Publisher] Create writer." << std::endl;

        /* First, a domain participant is needed.
         * Create one on the default domain. */
        dds::domain::DomainParticipant participant(domain::default_id());

        /* To publish something, a topic is needed. */
        dds::topic::Topic<ImageTransfer::ImageAndMetadata> topic(participant, "ImageAndMetaDataTopic");

        /* A writer also needs a publisher. */
        dds::pub::Publisher publisher(participant);

        /* Now, the writer can be created to publish a HelloWorld message. */
        dds::pub::DataWriter<ImageTransfer::ImageAndMetadata> writer(publisher, topic);

        /* For this example, we'd like to have a subscriber to actually read
         * our message. This is not always necessary. Also, the way it is
         * done here is just to illustrate the easiest way to do so. It isn't
         * really recommended to do a wait in a polling loop, however.
         * Please take a look at Listeners and WaitSets for much better
         * solutions, albeit somewhat more elaborate ones. */
        std::cout << "=== [Publisher] Waiting for subscriber." << std::endl;
        while (writer.publication_matched_status().current_count() == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        /* Create a message to write. */
        // dds_demo_data::Msg msg(1, "Hello World DDS demo");
        
        // Initialize a data instance
        
        ImageTransfer::ImageAndMetadata image_and_metadata;
        read_image_to_blob("../data/image.jpg", image_and_metadata.image_data());
        read_json_to_blob("../data/metadata.json", image_and_metadata.json_metadata());

        /* Write the message. */
        std::cout << "=== [Publisher] Write sample." << std::endl;
        writer.write(image_and_metadata);

        /* With a normal configuration (see dds::pub::qos::DataWriterQos
         * for various different writer configurations), deleting a writer will
         * dispose all its related message.
         * Wait for the subscriber to have stopped to be sure it received the
         * message. Again, not normally necessary and not recommended to do
         * this in a polling loop. */
        std::cout << "=== [Publisher] Waiting for sample to be accepted." << std::endl;
        while (writer.publication_matched_status().current_count() > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    catch (const dds::core::Exception& e) {
        std::cerr << "=== [Publisher] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "=== [Publisher] Done." << std::endl;

    return EXIT_SUCCESS;
}
